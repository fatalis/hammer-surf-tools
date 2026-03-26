#include "rampgen.h"
#include "rampgenui.h"
#include "util.h"
#include "hooks.h"

static RampGenState state;
static HWND dlg;
static bool generating;
static bool can_undo;
static bool window_initializing;

static Axis orientation_to_axis(FaceOrientation ori) {
    if (ori == FACE_ORIENTATION_SOUTH_WALL || ori == FACE_ORIENTATION_NORTH_WALL) {
        return AXIS_X;
    }
    if (ori == FACE_ORIENTATION_EAST_WALL || ori == FACE_ORIENTATION_WEST_WALL) {
        return AXIS_Y;
    }
    return AXIS_Z;
}

static void reset_ui(CMapSolid *solid) {
    state.ramp = solid;
    state.ui_degrees = 3.0f;
#ifdef RAMPGEN_DEBUG
    state.ui_segments = 1;
#else
    state.ui_segments = 30;
#endif
    state.curve = 'l';
    state.direction = DIR_PLUS;
    state.segment_gap = 0;

    Vec3 orig_size = BBoxSize(&solid->base.m_Render2DBox);
    state.segment_width = orig_size.v[state.axis];
}

static bool init_ramp_state() {
    const float ideal_normal = 0.64f;
    CMapSolid *solid = state.ramp;
    float best_normal_delta;
    int best_face = -1;

    for (auto i = 0; i < solid->Faces.length; i++) {
        CMapFace *face = &solid->Faces.items[i];
        FaceOrientation orientation = CMapFaceMethods.GetOrientation(face);
        float znorm = fabsf(face->plane.normal.z);
        float delta = fabsf(znorm - ideal_normal);
        Axis axis = orientation_to_axis(orientation);
        if ((best_face == -1 || delta < best_normal_delta) && axis != AXIS_Z && znorm < SURF_NORMAL && znorm > 0.0f) {
            best_face = i;
            best_normal_delta = delta;
        }
    }

    if (best_face != -1) {
        CMapFace *face = &solid->Faces.items[best_face];
        FaceOrientation orientation = CMapFaceMethods.GetOrientation(face);
        Axis axis = orientation_to_axis(orientation);
        char curve = state.curve;
        AppendDirection direction = state.direction;

        state.segments = state.ui_segments;

        state.axis = axis;
        state.orientation = orientation;

        int dir = (direction == DIR_PLUS) ? +1 : -1;
        int facing = (orientation == FACE_ORIENTATION_NORTH_WALL
                   || orientation == FACE_ORIENTATION_EAST_WALL) ? +1 : -1;

        int turn = (curve == 'l') - (curve == 'r'); // +1 for l, -1 for r, 0 otherwise
        int axis_sign = (axis == AXIS_X) ? +1 : -1;
        int sign = dir * facing * axis_sign;
        bool convex = (curve == 'd') || (turn * sign > 0);
        state.convex = convex;
        state.sign = sign;

        float degrees = state.ui_degrees;
        if (curve == 'l' || curve == 'r') {
            degrees *= (float)sign;
        }

        state.rotate_angle = ROLL;
        if (curve == 'u' || curve == 'd') {
            state.rotate_angle = axis == AXIS_X ? PITCH : YAW;
            // For up/down curves, Hammer's sign needs to depend on append
            // direction so that DIR_MINUS ramps flip the sign relative to
            // DIR_PLUS while keeping the same magnitude.
            degrees = -degrees * (float)dir;
        }
        state.degrees = degrees;

        // Precompute whether the edge used to build the cut plane should be
        // flipped (swap pivot/pivot_end) so that the chosen Split side stays
        // consistent across orientations/directions.
        bool flip_edge = false;
        if (convex && curve == 'r') {
            // For left/right convex curves we always flip.
            flip_edge = true;
        } else if (convex && curve == 'd' && sign > 0) {
            // For convex down curves, only flip on the "positive" sign cases.
            flip_edge = true;
        }
        state.flip_edge = flip_edge;

        // log_msg("axis:%d deg:%g rotate_angle:%d ori:%s dir:%d facing:%d turn:%d curve:%c convex:%d\n",
        //         axis, (double)degrees, ramp.rotate_angle,
        //         GetFaceOrientationStr(orientation), dir, facing, turn, curve, convex);

        const BoxCorner pivot_table[4][2] = {
            // minus, plus
            { NW, NE }, // FACE_ORIENTATION_NORTH_WALL
            { SW, SE }, // FACE_ORIENTATION_SOUTH_WALL
            { SE, NE }, // FACE_ORIENTATION_EAST_WALL
            { SW, NW }, // FACE_ORIENTATION_WEST_WALL
        };

        const BoxCorner pivot_end_table[4][2] = {
            { SW, SE }, // FACE_ORIENTATION_NORTH_WALL
            { NW, NE }, // FACE_ORIENTATION_SOUTH_WALL
            { SW, NW }, // FACE_ORIENTATION_EAST_WALL
            { SE, NE }, // FACE_ORIENTATION_WEST_WALL
        };

        // convex ramps pivot on the high end of the ramp
        // concave pivot on the short end
        bool c = convex && curve != 'd';
        const BoxCorner (*pt)[2] = c ? pivot_end_table : pivot_table;
        const BoxCorner (*pet)[2] = c ? pivot_table : pivot_end_table;

        int ori = (int)orientation - FACE_ORIENTATION_NORTH_WALL;
        state.pivot = pt[ori][direction];
        state.pivot_end = pet[ori][direction];
        state.pivot_opposite = pt[ori][!direction];
        state.pivot_opposite_end = pet[ori][!direction];

        return true;
    }

    return false;
}

static void rampgen_undo() {
    CHistory *history = GetHistory();

    // TODO: allow "Selection" undos and dont rampgen_close on them

    // should always be true since we have rampgen_close
    ASSERT(!strcmp(history->CurTrack->szName, "Ramp Generation"));

    CMapObjectList sel = {0};
    CMapObjectList unk = {0};
    CHistory_Undo(history, &sel, &unk);

    // other things ctrl+z does after undo:
    // RemoveDead, UpdateAllDependencies
}

static void rampgen_update() {
    // ignore CHistory_MarkUndoPosition hook calls that CHistory_Undo does
    // otherwise, rampgen_close would trigger on undo()
    generating = true;

    if (can_undo) {
        rampgen_undo();
    }

    init_ramp_state();
    rampgen(&state);
    can_undo = true;

    generating = false;
}

static void commit(bool select) {
    CMapDoc *doc = GetActiveMapDoc();

    if (doc && state.segment_list) {
        if (select) {
            CMapGroup *group = new_CMapGroup();
            ASSERT(group);

            for (auto i = 0; i < arrlen(state.segment_list); i++) {
                CMapSolid *seg = state.segment_list[i];
                seg->base.vtable->SetParent(seg, nullptr); // removes from it's parent too
                group->base.vtable->AddChild(group, (CMapClass *)seg);
            }
            doc->vtable->AddObjectToWorld(doc, group, nullptr);

            CMapObjectList list;
            list.items = (CMapClass **)&group;
            list.length = 1;
            CSelection_SelectObjectList(doc->m_pSelection, &list, scClear | scSelect);
        }

        arrfree(state.segment_list);
    }
}

static INT_PTR dlg_proc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
        case WM_INITDIALOG:
            SendDlgItemMessage(hDlg, IDC_DEGREES, UDM_SETRANGE32, 1, 100);
            SendDlgItemMessage(hDlg, IDC_DEGREES, UDM_SETPOS32, 0, (int)state.ui_degrees);

            SendDlgItemMessage(hDlg, IDC_SEGMENTS, UDM_SETRANGE32, 1, 1000);
            SendDlgItemMessage(hDlg, IDC_SEGMENTS, UDM_SETPOS32, 0, state.ui_segments);

            SendDlgItemMessage(hDlg, IDC_SEGMENT_WIDTH, UDM_SETRANGE32, 0, 2048);
            SendDlgItemMessage(hDlg, IDC_SEGMENT_WIDTH, UDM_SETPOS32, 0, (int)state.segment_width);

            SendDlgItemMessage(hDlg, IDC_SEGMENT_GAP, UDM_SETRANGE32, 0, 2048);
            SendDlgItemMessage(hDlg, IDC_SEGMENT_GAP, UDM_SETPOS32, 0, (int)state.segment_gap);

            // TODO: snap to 16 grid
            UDACCEL accel = { 0, 16 }; // 16 unit step - scroll only works on windows
            SendDlgItemMessage(hDlg, IDC_SEGMENT_WIDTH, UDM_SETACCEL, 1, (LPARAM)&accel);
            SendDlgItemMessage(hDlg, IDC_SEGMENT_GAP,   UDM_SETACCEL, 1, (LPARAM)&accel);

            if (state.curve == 'l') {
                CheckDlgButton(hDlg, IDC_CURVE_LEFT, BST_CHECKED);
            } else if (state.curve == 'r') {
                CheckDlgButton(hDlg, IDC_CURVE_RIGHT, BST_CHECKED);
            } else if (state.curve == 'u') {
                CheckDlgButton(hDlg, IDC_CURVE_UP, BST_CHECKED);
            } else if (state.curve == 'd') {
                CheckDlgButton(hDlg, IDC_CURVE_DOWN, BST_CHECKED);
            }
            CheckDlgButton(hDlg, state.direction == DIR_PLUS ? IDC_DIRECTION_PLUS : IDC_DIRECTION_MINUS, BST_CHECKED);

            window_initializing = false;
            can_undo = false;

            rampgen_update();

            return true;

        case WM_NOTIFY:
            NMHDR *hdr = (NMHDR*)lParam;
            if (hdr->code == UDN_DELTAPOS) {
                if (hdr->idFrom == IDC_DEGREES || hdr->idFrom == IDC_SEGMENTS || hdr->idFrom == IDC_SEGMENT_WIDTH || hdr->idFrom == IDC_SEGMENT_GAP) {
                    log_msg("delta %d\n", (int)window_initializing);
                    NMUPDOWN *ud = (NMUPDOWN*)lParam;
                    if (hdr->idFrom == IDC_DEGREES) {
                        state.ui_degrees = (float)(ud->iPos + ud->iDelta);
                    }
                    else if (hdr->idFrom == IDC_SEGMENTS) {
                        state.ui_segments = ud->iPos + ud->iDelta;
                    }
                    else if (hdr->idFrom == IDC_SEGMENT_WIDTH) {
                        state.segment_width = (float)(ud->iPos + ud->iDelta);
                    } else if (hdr->idFrom == IDC_SEGMENT_GAP) {
                        state.segment_gap = (float)(ud->iPos + ud->iDelta);
                    }

                    rampgen_update();
                    return true;
                }
            }
            break;

        case WM_COMMAND:
            int cmd = HIWORD(wParam);
            int id = LOWORD(wParam);

            if (cmd == BN_CLICKED) {
                if (id == IDC_CURVE_LEFT || id == IDC_CURVE_RIGHT || id == IDC_CURVE_UP || id == IDC_CURVE_DOWN) {
                    if (id == IDC_CURVE_LEFT) {
                        state.curve = 'l';
                    } else if (id == IDC_CURVE_RIGHT) {
                        state.curve = 'r';
                    } else if (id == IDC_CURVE_UP) {
                        state.curve = 'u';
                    } else /* if (id == IDC_CURVE_DOWN) */ {
                        state.curve = 'd';
                    }
                    rampgen_update();
                    return true;
                } else if (id == IDC_DIRECTION_PLUS || id == IDC_DIRECTION_MINUS) {
                    state.direction = id == IDC_DIRECTION_PLUS ? DIR_PLUS : DIR_MINUS;
                    rampgen_update();
                    return true;
                } else if (id == IDCANCEL || id == IDOK) {
                    if (id == IDCANCEL) {
                        rampgen_undo();
                        commit(false);
                    } else if (id == IDOK) {
                        commit(true);
                    }
                    DestroyWindow(hDlg);
                    dlg = nullptr;
                    return true;
                }
            } else if (cmd == EN_CHANGE && !window_initializing) {
                char buf[16];
                if (id == IDC_DEGREES_EDIT || id == IDC_SEGMENTS_EDIT || id == IDC_SEGMENT_WIDTH_EDIT || id == IDC_SEGMENT_GAP_EDIT) {
                    GetWindowText(GetDlgItem(dlg, id), buf, sizeof(buf));
                    int val = atoi(buf);

                    if (id == IDC_DEGREES_EDIT) {
                        state.ui_degrees = (float)val;
                    } else if (id == IDC_SEGMENTS_EDIT) {
                        state.ui_segments = val;
                    } else if (id == IDC_SEGMENT_WIDTH_EDIT) {
                        state.segment_width = (float)val;
                    } else if (id == IDC_SEGMENT_GAP_EDIT) {
                        state.segment_gap = (float)val;
                    }

                    rampgen_update();
                    return true;
                }
            }
            break;
        // case WM_CLOSE:
        //     DestroyWindow(hDlg);
        //     dlg = nullptr;
        //     return true;
        // case WM_KEYDOWN:
        //     if (wParam == VK_ESCAPE)
        //     {
        //         DestroyWindow(hDlg);
        //         dlg = nullptr;
        //         return true;
        //     }
        //     break;
    }

    return false;
}

static CMapSolid *get_selected_ramp() {
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

    CMapSolid *solid = (CMapSolid *)item;

    return solid;
}

void do_ramp_generator() {
    if (dlg) {
        return;
    }

    CMapSolid *solid = get_selected_ramp();
    if (!solid) {
        return;
    }

    reset_ui(solid);

    if (!init_ramp_state()) {
        AfxMessageBoxF(MB_OK, "Brush must have a surfable face running along x or y.");
        return;
    }

    window_initializing = true;
    dlg = CreateDialogA(
        GetHInstance(),
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
        // changes are comitted but selection is left alone
        commit(false);
        DestroyWindow(dlg);
        dlg = nullptr;
    }
}
