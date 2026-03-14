#include "rampgen.h"
#include "hooks.h"
#include "scriptfuncs.h"
#include "hammerfuncs.h"
#include "util.h"

// #define RAMPGEN_DEBUG
static RampGenCmd cmd;
static HWND dlg;
static bool generating;

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

static void undo() {
    CHistory *history = GetHistory();

    // should always be true since we have rampgen_close
    ASSERT(!strcmp(history->CurTrack->szName, "Ramp Generation"));

    CMapObjectList sel = {0};
    CMapObjectList unk = {0};
    CHistory_Undo(history, &sel, &unk);

    // other things ctrl+z does after undo:
    // RemoveDead, UpdateAllDependencies
}

// static void rampgen(CMapClass *solid, float degrees, int segments, char direction, float segment_width) {
static void rampgen(bool initial) {
    CMapDoc *doc = GetActiveMapDoc();
    ASSERT(doc);

    // ignore CHistory_MarkUndoPosition hook calls that CHistory_Undo does
    // otherwise, rampgen_close would trigger on undo()
    generating = true;

    if (!initial) {
        undo();
    }

    CMapClass *solid = cmd.ramp;
    float degrees = cmd.degrees;

    char axis = ramp_orientation(solid);
    ASSERT(axis == 'x' || axis == 'y');
    if (cmd.direction == 'r') {
        degrees = -degrees;
    }

    CHistory_MarkUndoPosition(GetHistory(), CMapDoc_GetSelection(doc), "Ramp Generation", false);
    CSelection_SelectObjectList(doc->m_pSelection, nullptr, scClear);
    CHistory_Keep(GetHistory(), solid);

    Vec3 orig_size;
    BBoxSize(&solid->m_Render2DBox, &orig_size);

    float factor = cmd.segment_width / (axis == 'x' ? orig_size.x : orig_size.y);
    BoundingBox *bbox = &solid->m_Render2DBox;
    // TODO: make a func for this
    Vec3 ref;
    if (axis == 'x') {
        ref = (Vec3){
            cmd.direction == 'l' ? bbox->maxs.x : bbox->mins.x,
            bbox->mins.y,
            bbox->mins.z
        };
    } else {
        ref = (Vec3){
            bbox->mins.x,
            cmd.direction == 'l' ? bbox->maxs.y : bbox->mins.y,
            bbox->mins.z
        };
    }
    Vec3 scale = {1.0f, 1.0f, 1.0f};
    if (axis == 'x') {
        scale.x = factor;
    } else {
        scale.y = factor;
    }

    TransScale(solid, &ref, &scale);

    // Vec3 orig_size;
    BBoxSize(&solid->m_Render2DBox, &orig_size);
    Vec3 orig_pos = solid->m_Origin;
    // log_msg("orig_pos %g %g %g\n", (double)orig_pos.x, (double)orig_pos.y, (double)orig_pos.z);


    int n_items = 0;
    CMapClass *items[cmd.segments + 1];

    items[0] = solid;
    n_items++;

    for (int seg = 1; seg <= cmd.segments; seg++) {
        CMapClass *prev_item = items[seg-1];
        CMapClass *copy = prev_item->vtable->Copy(prev_item, FALSE);

        items[n_items] = copy;
        n_items++;

        BoundingBox *bbox_prev = &prev_item->m_Render2DBox;

        Vec3 size;
        BBoxSize(bbox_prev, &size); // or use the selected solid's size
        // log_msg("seg %d: %g %g %g -> %g %g %g\n", seg, (double)orig_size.x, (double)orig_size.y, (double)orig_size.z,
                // (double)size.x, (double)size.y, (double)size.z);
        // ASSERT((int)size.x == (int)orig_size.x && (int)size.y == (int)orig_size.y && (int)size.z == (int)orig_size.z);

        // move new seg
        Vec3 delta = {0.0f, 0.0f, 0.0f};
        if (axis == 'x') {
            delta.x = cmd.direction == 'l' ? size.x : -size.x;
        } else {
            delta.y = cmd.direction == 'l' ? size.y : -size.y;
        }
        TransMove(copy, &delta);
#ifdef RAMPGEN_DEBUG
        Sleep(500);
#endif

        // rotate new seg on axis by bottom of previous seg
        Vec3 ref;
        if (axis == 'x') {
            ref = (Vec3){
                cmd.direction == 'l' ? bbox_prev->maxs.x : bbox_prev->mins.x,
                bbox_prev->mins.y,
                bbox_prev->mins.z
            };
        } else {
            ref = (Vec3){
                bbox_prev->mins.x,
                cmd.direction == 'l' ? bbox_prev->maxs.y : bbox_prev->mins.y,
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

        if (seg == cmd.segments) {
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

    ASSERT(n_items == cmd.segments + 1);
    for (auto i = 1; i < n_items; i++) {
        doc->vtable->AddObjectToWorld(doc, items[i], nullptr);
        CHistory_KeepNew(GetHistory(), items[i], false);
    }

    // TODO: do the select on IDOK click
    // CMapObjectList list;
    // list.items = items;
    // list.length = n_items;
    // CSelection_SelectObjectList(doc->m_pSelection, &list, scClear | scSelect);

    CMapDoc_SetModifiedFlag(doc, true);

    generating = false;
}

static CMapClass *get_selected_ramp() {
    CMapDoc *doc = GetActiveMapDoc();
    if (!doc) {
        return nullptr;
    }

    CMapObjectList *selected = CMapDoc_GetSelection(doc);

    if (selected->length != 1) {
        AfxMessageBoxF(MB_OK, "Selection should contain exactly 1 item.");
        return nullptr;
    }

    CMapClass *item = selected->items[0];
    ASSERT(item);

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
static DWORD WINAPI hook_init_thread(LPVOID param) {
    RampGenCmd *cmd = param;
    rampgen(cmd->ramp, cmd->degrees, cmd->segments, cmd->direction);
    free(cmd);
    return 0;
}
#endif

static INT_PTR dlg_proc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
        case WM_INITDIALOG:
            SendDlgItemMessage(hDlg, IDC_DEGREES, UDM_SETRANGE32, 1, 100);
            SendDlgItemMessage(hDlg, IDC_DEGREES, UDM_SETPOS32, 0, (int)cmd.degrees);

            SendDlgItemMessage(hDlg, IDC_SEGMENTS, UDM_SETRANGE32, 1, 1000);
            SendDlgItemMessage(hDlg, IDC_SEGMENTS, UDM_SETPOS32, 0, cmd.segments);

            SendDlgItemMessage(hDlg, IDC_SEGMENT_WIDTH, UDM_SETRANGE32, 0, 2048);
            SendDlgItemMessage(hDlg, IDC_SEGMENT_WIDTH, UDM_SETPOS32, 0, (int)cmd.segment_width);
            UDACCEL accel = { 0, 16 }; // 16 unit step - TODO: doesnt work with scroll?
            SendDlgItemMessage(hDlg, IDC_SEGMENT_WIDTH, UDM_SETACCEL, 1, (LPARAM)&accel);

            CheckDlgButton(hDlg, IDC_DIRECTION_LEFT, BST_CHECKED);

            rampgen(true);

            return true;

        case WM_NOTIFY:
            NMHDR *hdr = (NMHDR*)lParam;
            if (hdr->code == UDN_DELTAPOS) {
                if (hdr->idFrom == IDC_DEGREES || hdr->idFrom == IDC_SEGMENTS || hdr->idFrom == IDC_SEGMENT_WIDTH) {
                    NMUPDOWN *ud = (NMUPDOWN*)lParam;
                    if (hdr->idFrom == IDC_DEGREES) {
                        cmd.degrees = (float)(ud->iPos + ud->iDelta);
                    }
                    else if (hdr->idFrom == IDC_SEGMENTS) {
                        cmd.segments = ud->iPos + ud->iDelta;
                    }
                    else if (hdr->idFrom == IDC_SEGMENT_WIDTH) {
                        cmd.segment_width = (float)(ud->iPos + ud->iDelta);
                    }

                    rampgen(false);
                    return true;
                }
            }
            break;

        case WM_COMMAND:
            if (HIWORD(wParam) == BN_CLICKED) {
                int id = LOWORD(wParam);

                if (id == IDC_DIRECTION_LEFT || id == IDC_DIRECTION_RIGHT) {
                    cmd.direction = id == IDC_DIRECTION_LEFT ? 'l' : 'r';
                    rampgen(false);
                    return true;
                } else if (id == IDCANCEL || id == IDOK) {
                    if (id == IDCANCEL) {
                        undo();
                    }
                    DestroyWindow(hDlg);
                    dlg = nullptr;
                    return true;
                }
            }
            break;
    }

    return false;
}

void do_ramp_generator() {
    CMapClass *ramp = get_selected_ramp();
    if (!ramp) {
        return;
    }

    char axis = ramp_orientation(ramp);
    ASSERT(axis == 'x' || axis == 'y');
    Vec3 orig_size;
    BBoxSize(&ramp->m_Render2DBox, &orig_size);
    float width = axis == 'x' ? orig_size.x : orig_size.y;

    cmd = (RampGenCmd){ramp, 3.0f, 30, 'l', width};
    dlg = CreateDialogA(
        GetModuleHandleA("version.dll"),
        MAKEINTRESOURCE(IDD_RAMPGEN),
        GetMainWndHwnd(),
        dlg_proc
    );
    if (dlg) {
        ShowWindow(dlg, SW_SHOW);
    }
}

// called from CHistory_MarkUndoPosition hook
// ie when the user makes changes after a ramp gen, close
void rampgen_close() {
    if (dlg && !generating) {
        DestroyWindow(dlg);
        dlg = nullptr;
    }
}
