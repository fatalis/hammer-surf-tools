#include "rampgen.h"
#include "hooks.h"
#include "scriptfuncs.h"
#include "hammerfuncs.h"
#include "util.h"

// TODO: esc -> close
// TODO: add tests

static void debug(const char *msg) {
#ifdef RAMPGEN_DEBUG
    CMapDocMethods.UpdateAllViews(GetActiveMapDoc(), MAPVIEW_UPDATE_OBJECTS | MAPVIEW_RENDER_NOW, nullptr);
    AfxMessageBoxF(MB_OK, msg);
#endif
}

static inline Vec3 box_bottom_corner(const BoundingBox *bbox, BoxCorner corner) {
    float z = bbox->mins.z;

    if (corner == SW) {
        return (Vec3){{ bbox->mins.x, bbox->mins.y, z }};
    } else if (corner == SE) {
        return (Vec3){{ bbox->maxs.x, bbox->mins.y, z }};
    } else if (corner == NE) {
        return (Vec3){{ bbox->maxs.x, bbox->maxs.y, z }};
    } else { // NW
        return (Vec3){{ bbox->mins.x, bbox->maxs.y, z }};
    }
}

static inline Plane vertical_plane_from_bbox(const BoundingBox *bbox, RampGenState *state) {
    BoxCorner first  = state->flip_edge ? state->pivot      : state->pivot_end;
    BoxCorner second = state->flip_edge ? state->pivot_end  : state->pivot;

    Vec3 p0 = box_bottom_corner(bbox, first);
    Vec3 p1 = box_bottom_corner(bbox, second);

    // third point above edge
    Vec3 p2 = {{ p0.x, p0.y, bbox->maxs.z }};

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

static void resize_start_seg(CMapDoc *doc, CMapSolid *solid, RampGenState *state) {
    debug("scale seg");

    Vec3 orig_size = BBoxSize(&solid->base.m_Render2DBox);

    // scale the start seg
    log_msg("%g %g\n", (double)state->segment_width, (double)orig_size.v[state->axis]);
    float factor = state->segment_width / orig_size.v[state->axis];
    Vec3 ref = box_bottom_corner(&solid->base.m_Render2DBox, state->pivot_opposite);
    Vec3 scale = VEC3_ONE;
    scale.v[state->axis] = factor;

    TransScale(solid, &ref, &scale);
}

static void move_seg(CMapDoc *doc, CMapSolid *prev_seg, CMapSolid *seg, RampGenState *state) {
    debug("move seg");

    Vec3 size = BBoxSize(&prev_seg->base.m_Render2DBox); // or use the selected solid's size

    Vec3 delta = VEC3_ZERO;
    delta.v[state->axis] = state->direction == DIR_PLUS ? size.v[state->axis] : -size.v[state->axis];
    delta.v[state->axis] += state->direction == DIR_PLUS ? state->segment_gap : -state->segment_gap;

    TransMove(seg, &delta);
}

static void rotate_seg(CMapDoc *doc, CMapSolid *seg, CMapSolid *ref_ent, Angle angle, float degrees, BoxCorner pivot, bool top) {
    debug("rotate seg");

    Vec3 ref = box_bottom_corner(&ref_ent->base.m_Render2DBox, pivot);
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

static void move_back(CMapDoc *doc, Vec3 orig_pos, CMapSolid *seg, CMapSolid **segments, RampGenState *state) {
    debug("move_back: flip");
    Vec3 ref = VEC3_ZERO;
    Vec3 scale = VEC3_ONE;
    scale.v[state->axis] = -1.0f;
    for (auto i = 0; i < arrlen(segments); i++) {;
        TransScale(segments[i], &ref, &scale);
    }

    debug("move_back: move");
    Vec3 moved = vec3Subtract(orig_pos, seg->base.point.m_Origin);

    for (auto i = 0; i < arrlen(segments); i++) {;
        TransMove(segments[i], &moved);
    }
}

static CMapSolid *cut_convex_seg(CMapDoc *doc, CMapSolid *solid, RampGenState *state) {
    Plane plane = vertical_plane_from_bbox(&solid->base.m_Render2DBox, state);
#ifdef CONVEX_DEBUG
    debug_point(502, &plane.points[0], 0x00ffff);
    debug_point(502, &plane.points[1], 0x00ffff);
    debug_point(503, &plane.points[2], 0x00ffff);
#endif

    float degrees = state->degrees;
    BoxCorner pivot = state->pivot;
    Angle rotate_angle = state->rotate_angle;
    bool top = state->curve == 'd';

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
    scale.v[state->axis] = -1.0f;
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
    CMapDocMethods.DeleteObject(doc, (CMapClass *)cut);
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

void rampgen_impl(RampGenState *state) {
    CMapDoc *doc = GetActiveMapDoc();
    ASSERT(doc);

    CMapSolid *solid = state->ramp;
    int wish_segments = state->segments;

//     solid = (CMapSolid *)solid->base.vtable->Copy(solid, false);
// #ifdef RAMPGEN_DEBUG
//     doc->vtable->AddObjectToWorld(doc, solid, nullptr);
// #endif

    // scale start seg to new width
    resize_start_seg(doc, solid, state);

    if (state->convex) {
        // cut 2 half rotation sides so we get a repeatable non-overlapping segment
        solid = cut_convex_seg(doc, solid, state);
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
        move_seg(doc, prev_seg, new_seg, state);

        // rotate new seg
        float degrees = state->convex ? state->degrees : -state->degrees;
        bool top = state->curve == 'd';
        rotate_seg(doc, new_seg, prev_seg, state->rotate_angle, degrees, state->pivot, top);

        // rotate all segs including new seg back so that the new seg is axis aligned again
        rotate_all_segs(doc, segments, state->rotate_angle, -degrees);

        // flip and move back to initial segment pos
        if (seg == wish_segments) {
            move_back(doc, orig_pos, new_seg, segments, state);
        }
    }

    ASSERT(arrlen(segments) == wish_segments + 1);
    // TODO: change to 0 when using copied seg
    for (auto seg = 1; seg < arrlen(segments); seg++) {
#ifndef RAMPGEN_DEBUG
        doc->vtable->AddObjectToWorld(doc, segments[seg], nullptr);
#endif
        CHistory_KeepNew(GetHistory(), (CMapClass *)segments[seg], false);
    }

    if (state->segment_list) {
        arrfree(state->segment_list);
    }
    state->segment_list = segments;
}

void rampgen(RampGenState *state) {
    CMapDoc *doc = GetActiveMapDoc();
    ASSERT(doc);

    CHistory_MarkUndoPosition(GetHistory(), CMapDoc_GetSelection(doc), "Ramp Generation", false);
    CSelection_SelectObjectList(doc->m_pSelection, nullptr, scClear);
    CHistory_Keep(GetHistory(), (CMapClass *)state->ramp);

    rampgen_impl(state);

    CMapDocMethods.SetModifiedFlag(doc, true);
}
