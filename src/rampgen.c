#include <assert.h>

#include "common.h"
#include "rampgen.h"
#include "hooks.h"
#include "render.h"
#include "scriptfuncs.h"

// #define RAMPGEN_DEBUG
static const char help[] = "Expecting <direction: l/r> <degrees> <segments>\r\nExample: l 3 30";

static char OrientationToAxis(FaceOrientation ori) {
    if (ori == FACE_ORIENTATION_SOUTH_WALL || ori == FACE_ORIENTATION_NORTH_WALL) {
        return 'x';
    }
    if (ori == FACE_ORIENTATION_EAST_WALL || ori == FACE_ORIENTATION_WEST_WALL) {
        return 'y';
    }
    return 'z';
}

static char GetRampOrientation(CMapClass *solid) {
    // find best surfable face
    CMapFace *best = nullptr;
    float best_normal_delta;
    char best_axis;
    const float ideal_normal = 0.64f;

    for (auto i = 0; i < solid->Faces.length; i++) {
        CMapFace *face = &solid->Faces.list[i];
        float znorm = fabsf(face->plane.normal.z);
        float delta = fabsf(znorm - ideal_normal);
        char axis = OrientationToAxis(CMapFace_GetOrientation(face));
        if ((!best || delta < best_normal_delta) && axis != 'z' && znorm < SURF_NORMAL && znorm > 0.0f) {
            best = face;
            best_normal_delta = delta;
            best_axis = axis;
        }
    }

    if (best) {
        return best_axis;
    }

    return '?';
}

static void rampgen(float degrees, int segments, char direction) {
    CMapDoc *doc = GetActiveMapDoc();
    if (!doc || !selection3d) {
        return;
    }

    if (direction == 'r') {
        degrees = -degrees;
    }

    void *selection = *(void **)((void *)selection3d + SELECTION3D_OFFSET_SELECTION);
    RefVector *selected = ((void *)selection + SELECTION_OFFSET_SEL_LIST);

    log_msg("[hook] %p sel len %d\n", selected, selected->length);

    if (selected->length != 1) {
        log_msg("[hook] error: selection should be exactly 1 item\n");
        return;
    }

    CMapClass *item = selected->items[0];
    assert(item);
    char *name = item->vtable->GetType(item);

    if (!strcmp(name, "CMapSolid")) {
        char axis = GetRampOrientation(item);
        if (axis == '?') {
            AfxMessageBoxF(MB_OK, "Solid must have a surfable face facing x or y direction.");
            return;
        }

        CHistory_MarkUndoPosition(GetHistory(), nullptr, "Ramp Generation", false);

        Vec3 orig_size;
        BBoxSize(&item->m_Render2DBox, &orig_size);
        Vec3 orig_pos = item->m_Origin;

        int n_items = 0;
        CMapClass *items[segments + 1];

        CHistory_Keep(GetHistory(), item);

        items[0] = item;
        n_items++;

        for (int seg = 1; seg <= segments; seg++) {
            CMapClass *prev_item = items[seg-1];
            CMapClass *copy = prev_item->vtable->Copy(prev_item, FALSE);

            items[n_items] = copy;
            n_items++;

            BoundingBox *bbox_prev = &prev_item->m_Render2DBox;

            Vec3 size;
            BBoxSize(bbox_prev, &size); // or use the selected item's size
            log_msg("%g %g %g -> %g %g %g\n", (double)orig_size.x, (double)orig_size.y, (double)orig_size.z,
                    (double)size.x, (double)size.y, (double)size.z);
            // assert((int)size.x == (int)orig_size.x && (int)size.y == (int)orig_size.y && (int)size.z == (int)orig_size.z);

            // move new seg
            Vec3 delta = {0.0f, 0.0f, 0.0f};
            if (axis == 'x') {
                delta.x = direction == 'l' ? size.x : -size.x;
            } else if (axis == 'y') {
                delta.y = size.y;
            } else if (axis == 'z') {
                delta.z = size.z;
            }
            TransMove(copy, &delta);
#ifdef RAMPGEN_DEBUG
            Sleep(500);
#endif

            // rotate new seg on axis by bottom of previous seg
            // adjust for x/y/z
            Vec3 ref;
            if (axis == 'x') {
                ref = (Vec3){
                    direction == 'l' ? bbox_prev->maxs.x : bbox_prev->mins.x,
                    bbox_prev->mins.y,
                    bbox_prev->mins.z
                };
            } else if (axis == 'y') {
                ref = (Vec3){
                    bbox_prev->mins.x,
                    bbox_prev->maxs.y,
                    bbox_prev->mins.z
                };
            } else if (axis == 'z') {
                ref = (Vec3){
                    bbox_prev->mins.x,
                    bbox_prev->mins.y,
                    bbox_prev->maxs.z
                };
            }

            Euler angles = {0.0f, 0.0f, 0.0f};
            if (axis == 'x') {
                angles.pitch = degrees;
            } else if (axis == 'y') {
                angles.yaw = degrees;
            } else if (axis == 'z') {
                angles.roll = degrees;
            }
            TransRotate(copy, &angles, &ref);
#ifdef RAMPGEN_DEBUG
            Sleep(500);
#endif

            // rotate all segs including new seg back so that the new seg is axis aligned for next seg to copy and move
            BBoxTrueCenter(*items, n_items, &ref);
            for (auto item_idx = 0; item_idx < n_items; item_idx++) {
                Euler angles = {0.0f, 0.0f, 0.0f};
                if (axis == 'x') {
                    angles.pitch = -degrees;
                } else if (axis == 'y') {
                    angles.yaw = -degrees;
                } else if (axis == 'z') {
                    angles.roll = -degrees;
                }
                TransRotate(items[item_idx], &angles, &ref);
#ifdef RAMPGEN_DEBUG
                Sleep(30);
#endif
            }

#ifdef RAMPGEN_DEBUG
            Sleep(1000);
#endif

            if (seg == segments) {
                Vec3 moved = {
                    orig_pos.x - copy->m_Origin.x,
                    orig_pos.y - copy->m_Origin.y,
                    orig_pos.z - copy->m_Origin.z,
                };

                for (auto i = 0; i < n_items; i++) {
                    TransMove(items[i], &moved);
                }
            }
        }

        assert(n_items == segments + 1);
        for (auto i = 1; i < n_items; i++) {
            doc->vtable->AddObjectToWorld(doc, items[i], nullptr);
            CHistory_KeepNew(GetHistory(), items[i], false);
        }
    } else {
        log_msg("[hook] error: selection wasnt a solid\n");
    }
}

static CStrDlgInst dlg;

#ifdef RAMPGEN_DEBUG
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
typedef struct {
    float degrees;
    int segments;
    char direction;
} RampGenCmd;

static DWORD WINAPI hook_init_thread(LPVOID param) {
    RampGenCmd *cmd = param;
    rampgen(cmd->degrees, cmd->segments, cmd->direction);
    free(cmd);
    return 0;
}
#endif

void do_ramp_generator() {
    memset(&dlg, 0, sizeof(dlg));
    CStrDlg(&dlg, 0, nullptr, help, "Curved Ramp Generator");

    if (DoModal(&dlg) == 1) {
        float degrees;
        int segments;
        char direction;

        // sscanf format: a character (x/y/z) followed by two integers
        if (sscanf(dlg.str, " %c %f %d", &direction, &degrees, &segments) == 3 && segments > 0) {
#ifdef RAMPGEN_DEBUG
                RampGenCmd *cmd = malloc(sizeof(RampGenCmd));
                *cmd = (RampGenCmd){degrees, segments, direction};
                CreateThread(nullptr, 0, hook_init_thread, cmd, 0, nullptr);
#else
                rampgen(degrees, segments, direction);
#endif
        } else {
            AfxMessageBoxF(MB_OK, help);
        }
    }
}
