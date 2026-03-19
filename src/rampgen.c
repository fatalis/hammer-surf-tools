#include "rampgen.h"
#include "hooks.h"
#include "scriptfuncs.h"
#include "hammerfuncs.h"
#include "util.h"

// TODO: esc -> close

static void debug(const char *msg) {
#ifdef RAMPGEN_DEBUG
    CMapDoc_UpdateAllViews(GetActiveMapDoc(), MAPVIEW_UPDATE_OBJECTS | MAPVIEW_RENDER_NOW, nullptr);
    AfxMessageBoxF(MB_OK, msg);
#endif
}

static void box_bottom_corner(const BoundingBox *bbox, BoxCorner corner, Vec3 *out) {
    float z = bbox->mins.z;

    if (corner == SW) {
        *out = (Vec3){{ bbox->mins.x, bbox->mins.y, z }};
    } else if (corner == SE) {
        *out = (Vec3){{ bbox->maxs.x, bbox->mins.y, z }};
    } else if (corner == NE) {
        *out = (Vec3){{ bbox->maxs.x, bbox->maxs.y, z }};
    } else /* if (corner == NW) */ {
        *out = (Vec3){{ bbox->mins.x, bbox->maxs.y, z }};
    }
}

static inline Plane vertical_plane_from_bbox(const BoundingBox *bbox, RampOrientation *ori) {
    BoxCorner first  = ori->flip_edge ? ori->pivot      : ori->pivot_end;
    BoxCorner second = ori->flip_edge ? ori->pivot_end  : ori->pivot;

    Vec3 p0, p1, p2;
    box_bottom_corner(bbox, first, &p0);
    box_bottom_corner(bbox, second, &p1);

    // third point above edge
    p2 = (Vec3){{ p0.x, p0.y, bbox->maxs.z }};

    // direction along edge
    Vec3 dir = {{
        p1.x - p0.x,
        p1.y - p0.y,
        0.0f
    }};

    // perpendicular vertical plane normal
    Vec3 normal = {{
        -dir.y,
        dir.x,
        0.0f
    }};

    ASSERT(vec3Length(normal) != 0.0f);
    normal = vec3Normalize(normal);
    float d = vec3DotProduct(normal, p0);

    Plane plane;
    plane.normal = normal;
    plane.dist = d;
    plane.points[0] = p0;
    plane.points[1] = p1;
    plane.points[2] = p2;

    return plane;
}

static Axis orientation_to_axis(FaceOrientation ori) {
    if (ori == FACE_ORIENTATION_SOUTH_WALL || ori == FACE_ORIENTATION_NORTH_WALL) {
        return AXIS_X;
    }
    if (ori == FACE_ORIENTATION_EAST_WALL || ori == FACE_ORIENTATION_WEST_WALL) {
        return AXIS_Y;
    }
    return AXIS_Z;
}

bool ramp_orientation(RampGenCmd *cmd, RampOrientation *out_orientation) {
    const float ideal_normal = 0.64f;
    CMapSolid *solid = cmd->ramp;
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
        char curve = cmd->curve;
        AppendDirection direction = cmd->direction;

        out_orientation->axis = axis;
        out_orientation->direction = direction;
        out_orientation->orientation = orientation;
        out_orientation->curve = curve;
        out_orientation->segment_gap = cmd->segment_gap;

        int dir = (direction == DIR_PLUS) ? +1 : -1;
        int facing = (orientation == FACE_ORIENTATION_NORTH_WALL
                   || orientation == FACE_ORIENTATION_EAST_WALL) ? +1 : -1;

        int turn = (curve == 'l') - (curve == 'r'); // +1 for l, -1 for r, 0 otherwise
        int axis_sign = (axis == AXIS_X) ? +1 : -1;
        int sign = dir * facing * axis_sign;
        bool convex = (curve == 'd') || (turn * sign > 0);
        out_orientation->convex = convex;
        out_orientation->sign = sign;

        float degrees = cmd->degrees;
        if (curve == 'l' || curve == 'r') {
            degrees *= (float)sign;
        }

        out_orientation->rotate_angle = ROLL;
        if (curve == 'u' || curve == 'd') {
            out_orientation->rotate_angle = axis == AXIS_X ? PITCH : YAW;
            // For up/down curves, Hammer's sign needs to depend on append
            // direction so that DIR_MINUS ramps flip the sign relative to
            // DIR_PLUS while keeping the same magnitude.
            degrees = -degrees * (float)dir;
        }
        out_orientation->degrees = degrees;

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
        out_orientation->flip_edge = flip_edge;

        // log_msg("axis:%d deg:%g rotate_angle:%d ori:%s dir:%d facing:%d turn:%d curve:%c convex:%d\n",
        //         axis, (double)degrees, out_orientation->rotate_angle,
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
        out_orientation->pivot = pt[ori][direction];
        out_orientation->pivot_end = pet[ori][direction];
        out_orientation->pivot_opposite = pt[ori][!direction];
        out_orientation->pivot_opposite_end = pet[ori][!direction];

        return true;
    }

    return false;
}

static void resize_start_seg(CMapDoc * doc, CMapSolid *solid, RampOrientation *ori, float segment_width) {
    debug("scale seg");

    Vec3 orig_size = BBoxSize(&solid->base.m_Render2DBox);

    // scale the start seg
    float factor = segment_width / orig_size.v[ori->axis];
    Vec3 ref;
    box_bottom_corner(&solid->base.m_Render2DBox, ori->pivot_opposite, &ref);
    Vec3 scale = VEC3_ONE;
    scale.v[ori->axis] = factor;

    TransScale(solid, &ref, &scale);
}

static void move_seg(CMapDoc *doc, CMapSolid *prev_seg, CMapSolid *seg, RampOrientation *ori) {
    debug("move seg");

    Vec3 size = BBoxSize(&prev_seg->base.m_Render2DBox); // or use the selected solid's size

    Vec3 delta = VEC3_ZERO;
    delta.v[ori->axis] = ori->direction == DIR_PLUS ? size.v[ori->axis] : -size.v[ori->axis];
    delta.v[ori->axis] += ori->direction == DIR_PLUS ? ori->segment_gap : -ori->segment_gap;

    TransMove(seg, &delta);
}

static void rotate_seg(CMapDoc *doc, CMapSolid *seg, CMapSolid *ref_ent, Angle angle, float degrees, BoxCorner pivot, bool top) {
    debug("rotate seg");

    Vec3 ref;
    box_bottom_corner(&ref_ent->base.m_Render2DBox, pivot, &ref);
    if (top) {
        ref.z = ref_ent->base.m_Render2DBox.maxs.z;
    }
#ifdef RAMPGEN_DEBUG
    debug_point(1, &ref, 0x00ff00);
#endif

    Euler angles = EULER_ZERO;
    angles.v[angle] = degrees;
    TransRotate(seg, &angles, &ref);
}

static void rotate_all_segs(CMapDoc *doc, CMapSolid **segments, Angle rotate_angle, float degrees) {
    debug("rotate all segs");

    Vec3 ref = BBoxTrueCenter((CMapClass **)segments);
    for (auto i = 0; i < arrlen(segments); i++) {
        Euler angles = EULER_ZERO;
        angles.v[rotate_angle] = degrees;
        TransRotate(segments[i], &angles, &ref);
    }
}

static void move_back(CMapDoc *doc, Vec3 orig_pos, CMapSolid *seg, CMapSolid **segments, RampOrientation *ori) {
    debug("move_back: flip");
    Vec3 ref = VEC3_ZERO;
    Vec3 scale = VEC3_ONE;
    scale.v[ori->axis] = -1.0f;
    for (auto i = 0; i < arrlen(segments); i++) {;
        TransScale(segments[i], &ref, &scale);
    }

    debug("move_back: move");
    Vec3 moved = vec3Subtract(orig_pos, seg->base.point.m_Origin);

    for (auto i = 0; i < arrlen(segments); i++) {;
        TransMove(segments[i], &moved);
    }
}

static CMapSolid *cut_convex_seg(CMapDoc *doc, CMapSolid *solid, RampOrientation *ori) {
    Plane plane = vertical_plane_from_bbox(&solid->base.m_Render2DBox, ori);
#ifdef CONVEX_DEBUG
    debug_point(502, &plane.points[0], 0x00ffff);
    debug_point(502, &plane.points[1], 0x00ffff);
    debug_point(503, &plane.points[2], 0x00ffff);
#endif

    float degrees = ori->degrees;
    BoxCorner pivot = ori->pivot;
    Angle rotate_angle = ori->rotate_angle;
    bool top = ori->curve == 'd';

    degrees /= 2.0f;

    debug("cut_convex_seg");
    rotate_seg(doc, solid, solid, rotate_angle, -degrees, pivot, top);

#ifdef CONVEX_DEBUG
    debug("convex: cut 1");
#endif
    CMapSolid *cut = nullptr;
    CMapSolidMethods.Split(solid, &plane, nullptr, &cut);
    if (!cut) {
        AfxMessageBoxF(MB_OK, "Convex: Cut 1 failed");
        return nullptr;
    }
#ifdef RAMPGEN_DEBUG
    doc->vtable->AddObjectToWorld(doc, cut, nullptr);
#endif
    CMapDocMethods.DeleteObject(doc, (CMapClass *)solid);

#ifdef CONVEX_DEBUG
    debug("convex: rotate back");
#endif
    rotate_seg(doc, cut, cut, rotate_angle, degrees, pivot, top);

    Vec3 scale = VEC3_ONE;
    scale.v[ori->axis] = -1.0f;
    Vec3 center = BBoxCenter(&cut->base.m_Render2DBox);
#ifdef CONVEX_DEBUG
    debug("convex: flip");
#endif
    TransScale(cut, &center, &scale);

#ifdef CONVEX_DEBUG
    debug("convex: rotate 2");
#endif
    rotate_seg(doc, cut, cut, rotate_angle, -degrees, pivot, top);

#ifdef CONVEX_DEBUG
    debug("convex: cut 2");
#endif
    CMapSolid *cut2 = nullptr;
    CMapSolidMethods.Split(cut, &plane, nullptr, &cut2);
    if (!cut2) {
        AfxMessageBoxF(MB_OK, "Convex: Cut 2 failed");
        return nullptr;
    }
#ifdef RAMPGEN_DEBUG
    doc->vtable->AddObjectToWorld(doc, cut2, nullptr);
    CMapDoc_DeleteObject(doc, (CMapClass *)cut);
#else
    cut->base.vtable->Dtor(cut, DELETE_OBJ);
#endif

#ifdef CONVEX_DEBUG
    debug("convex: rotate 3");
#endif
    rotate_seg(doc, cut2, cut2, rotate_angle, degrees, pivot, top);
    cut2->base.m_bTemporary = false; // required for CHistory_Keep*
#ifndef RAMPGEN_DEBUG
    doc->vtable->AddObjectToWorld(doc, cut2, nullptr);
#endif
    CHistory_KeepNew(GetHistory(), (CMapClass *)cut2, false);

    return cut2;
}

void rampgen_undo() {
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

void rampgen(RampGenCmd *cmd, RampOrientation *ori, bool initial, bool *generating) {
    CMapDoc *doc = GetActiveMapDoc();
    ASSERT(doc);

    // ignore CHistory_MarkUndoPosition hook calls that CHistory_Undo does
    // otherwise, rampgen_close would trigger on undo()
    *generating = true;

    if (!initial) {
        rampgen_undo();
        ASSERT(ramp_orientation(cmd, ori));
    }

    CMapSolid *solid = cmd->ramp;
    int wish_segments = cmd->segments;
    float segment_width = cmd->segment_width;

    CHistory_MarkUndoPosition(GetHistory(), CMapDoc_GetSelection(doc), "Ramp Generation", false);
    CSelection_SelectObjectList(doc->m_pSelection, nullptr, scClear);
    CHistory_Keep(GetHistory(), (CMapClass *)solid);

    // scale start seg to new width
    resize_start_seg(doc, solid, ori, segment_width);

    if (ori->convex) {
        // cut 2 half rotation sides so we get a repeatable non-overlapping segment
        solid = cut_convex_seg(doc, solid, ori);
        if (!solid) {
            return;
        }
    }

    Vec3 orig_pos = solid->base.point.m_Origin; // copy

    CMapSolid **segments = nullptr;
    arrput(segments, solid);

    for (int seg = 1; seg <= wish_segments; seg++) {
        // copy seg
        CMapSolid *prev_seg = segments[seg-1];
        CMapSolid *new_seg = (CMapSolid *)prev_seg->base.vtable->Copy(prev_seg, false);

#ifdef RAMPGEN_DEBUG
        // add now if debugging else later
        doc->vtable->AddObjectToWorld(doc, new_seg, nullptr);
#endif

        arrput(segments, new_seg);

        // move the new seg to the side of the prev seg
        move_seg(doc, prev_seg, new_seg, ori);

        // rotate new seg
        float degrees = ori->convex ? ori->degrees : -ori->degrees;
        bool top = ori->curve == 'd';
        rotate_seg(doc, new_seg, prev_seg, ori->rotate_angle, degrees, ori->pivot, top);

        // rotate all segs including new seg back so that the new seg is axis aligned again
        rotate_all_segs(doc, segments, ori->rotate_angle, -degrees);

        // flip and move back to initial segment pos
        if (seg == wish_segments) {
            move_back(doc, orig_pos, new_seg, segments, ori);
        }
    }

    ASSERT(arrlen(segments) == wish_segments + 1);
    for (auto i = 1; i < arrlen(segments); i++) {
#ifndef RAMPGEN_DEBUG
        doc->vtable->AddObjectToWorld(doc, segments[i], nullptr);
#endif
        CHistory_KeepNew(GetHistory(), (CMapClass *)segments[i], false);
    }

    CMapDocMethods.SetModifiedFlag(doc, true);

    if (ori->segment_list) {
        arrfree(ori->segment_list);
    }
    ori->segment_list = segments;

    *generating = false;
}

CMapSolid *get_selected_ramp() {
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
