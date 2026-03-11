#include "common.h"
#include "rampgen.h"
#include "hooks.h"
#include "scriptfuncs.h"
#include "hammerfuncs.h"
#include "util.h"

// #define RAMPGEN_DEBUG
static const char help[] = "Expecting <direction: l/r> <degrees> <segments>\r\nExample: r 3 20";

static char orientation_to_axis(FaceOrientation ori) {
    if (ori == FACE_ORIENTATION_SOUTH_WALL || ori == FACE_ORIENTATION_NORTH_WALL) {
        return 'x';
    }
    if (ori == FACE_ORIENTATION_EAST_WALL || ori == FACE_ORIENTATION_WEST_WALL) {
        return 'y';
    }
    return 'z';
}

static char ramp_orientation(CMapClass *solid) {
    // find best surfable face
    CMapFace *best = nullptr;
    float best_normal_delta;
    char best_axis;
    const float ideal_normal = 0.64f;

    for (auto i = 0; i < solid->Faces.length; i++) {
        CMapFace *face = &solid->Faces.items[i];
        float znorm = fabsf(face->plane.normal.z);
        float delta = fabsf(znorm - ideal_normal);
        char axis = orientation_to_axis(CMapFace_GetOrientation(face));
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

static void rampgen(CMapClass *solid, float degrees, int segments, char direction) {
    CMapDoc *doc = GetActiveMapDoc();
    if (!doc) {
        return;
    }

    char axis = ramp_orientation(solid);
    assert(axis == 'x' || axis == 'y');
    if (direction == 'r') {
        degrees = -degrees;
    }

    CHistory_MarkUndoPosition(GetHistory(), nullptr, "Ramp Generation", false);

    Vec3 orig_size;
    BBoxSize(&solid->m_Render2DBox, &orig_size);
    Vec3 orig_pos = solid->m_Origin;
    // log_msg("orig_pos %g %g %g\n", (double)orig_pos.x, (double)orig_pos.y, (double)orig_pos.z);

    int n_items = 0;
    CMapClass *items[segments + 1];

    CHistory_Keep(GetHistory(), solid);

    items[0] = solid;
    n_items++;

    for (int seg = 1; seg <= segments; seg++) {
        CMapClass *prev_item = items[seg-1];
        CMapClass *copy = prev_item->vtable->Copy(prev_item, FALSE);

        items[n_items] = copy;
        n_items++;

        BoundingBox *bbox_prev = &prev_item->m_Render2DBox;

        Vec3 size;
        BBoxSize(bbox_prev, &size); // or use the selected solid's size
        // log_msg("seg %d: %g %g %g -> %g %g %g\n", seg, (double)orig_size.x, (double)orig_size.y, (double)orig_size.z,
                // (double)size.x, (double)size.y, (double)size.z);
        // assert((int)size.x == (int)orig_size.x && (int)size.y == (int)orig_size.y && (int)size.z == (int)orig_size.z);

        // move new seg
        Vec3 delta = {0.0f, 0.0f, 0.0f};
        if (axis == 'x') {
            delta.x = direction == 'l' ? size.x : -size.x;
        } else {
            delta.y = direction == 'l' ? size.y : -size.y;
        }
        TransMove(copy, &delta);
#ifdef RAMPGEN_DEBUG
        Sleep(500);
#endif

        // rotate new seg on axis by bottom of previous seg
        Vec3 ref;
        if (axis == 'x') {
            ref = (Vec3){
                direction == 'l' ? bbox_prev->maxs.x : bbox_prev->mins.x,
                bbox_prev->mins.y,
                bbox_prev->mins.z
            };
        } else {
            ref = (Vec3){
                bbox_prev->mins.x,
                direction == 'l' ? bbox_prev->maxs.y : bbox_prev->mins.y,
                bbox_prev->mins.z
            };
        }

        Euler angles = {0.0f, 0.0f, 0.0f};
        if (axis == 'x') {
            angles.pitch = degrees;
        } else {
            angles.yaw = degrees;
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
            } else {
                angles.yaw = -degrees;
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

            // log_msg("moved %g %g %g\n", (double)moved.x, (double)moved.y, (double)moved.z);

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

    CMapDoc_SetModifiedFlag(doc, true);
}

static CMapClass *get_selected_ramp() {
    CMapDoc *doc = GetActiveMapDoc();
    if (!doc) {
        return nullptr;
    }

    MapClassPtrVector *selected = CMapDoc_GetSelection(doc);

    if (selected->length != 1) {
        AfxMessageBoxF(MB_OK, "Selection should contain exactly 1 item.");
        return nullptr;
    }

    CMapClass *item = selected->items[0];
    assert(item);

    if (!CMapClass_IsWorldBrush(item)) {
        AfxMessageBoxF(MB_OK, "Selection should be a world brush.");
        return nullptr;
    }

    char axis = ramp_orientation(item);
    if (axis == '?') {
        AfxMessageBoxF(MB_OK, "Brush must have a surfable face facing x or y direction.");
        return nullptr;
    }

    return item;
}

#ifdef RAMPGEN_DEBUG
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
typedef struct {
    CMapClass *ramp;
    float degrees;
    int segments;
    char direction;
} RampGenCmd;

static DWORD WINAPI hook_init_thread(LPVOID param) {
    RampGenCmd *cmd = param;
    rampgen(cmd->ramp, cmd->degrees, cmd->segments, cmd->direction);
    free(cmd);
    return 0;
}
#endif

void do_ramp_generator() {
    CMapClass *ramp = get_selected_ramp();
    if (!ramp) {
        return;
    }

    CStrDlgInst dlg;
    CStrDlg(&dlg, 0, nullptr, help, "Curved Ramp Generator");

    if (DoModal(&dlg) == 1) {
        float degrees;
        int segments;
        char direction;

        // sscanf format: a character (x/y/z) followed by two integers
        if (sscanf(dlg.m_string, " %c %f %d", &direction, &degrees, &segments) == 3 && segments > 0) {
            // TODO: verify input further
#ifdef RAMPGEN_DEBUG
            RampGenCmd *cmd = malloc(sizeof(RampGenCmd));
            *cmd = (RampGenCmd){ramp, degrees, segments, direction};
            CreateThread(nullptr, 0, hook_init_thread, cmd, 0, nullptr);
#else
            rampgen(ramp, degrees, segments, direction);
#endif
        } else {
            AfxMessageBoxF(MB_OK, help);
        }
    }
}
