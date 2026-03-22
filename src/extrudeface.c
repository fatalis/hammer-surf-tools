#include "extrudeface.h"
#include "hooks.h"
#include "hammerfuncs.h"
#include "util.h"

#define CREATE_BUILD_PLANE_POINTS   0x0001
#define CREATE_FROM_PLANES_CLIPPING 0x0002
#define CREATE_ALREADY_HAS_POINTS   0x0004

#define CAMERA_FORWARD 2

typedef struct {
    bool grabbing;
    CMapFace *face;
    CMapSolid *ent;
    float orig_dist;
} GrabInfo;

static GrabInfo grab;
static Vec2 last_pos;
static Debounce debounce = {1000/60};

CCamera *get_camera(CMapView3D *view) {
    CCameraVector *cameras = *(CCameraVector **)((void *)view + CMAPVIEW_OFFSET_CAMERALIST);
    if (cameras && cameras->length >= 1) {
        return &cameras->items[0];
    }
    return nullptr;
}

void do_extrude_face() {
    CMapDoc *doc = GetActiveMapDoc();
    if (!doc) {
        return;
    }

    if (grab.grabbing) {
        extrude_face_close();
    } else {
        CMapView *view = CMapDocMethods.GetActiveMapView(doc);
        if (!view) {
            return;
        }

        // TODO: make CMapView structs
        void *base = (void *)view - CMAPVIEW2D_OFFSET_MAPVIEW;
        void **map_view_vtable = *(void ***)base;
        CMapAtom_GetType_t GetType = map_view_vtable[0]; // not actual function type
        const char **typ = (const char **)GetType(base); // actually returns ptr to structs of { char* name, int size_of, int unk, void* ctor }, last one is all nulls
        if (!typ || strcmp(*typ, "CMapView3D") != 0) {
            return;
        }

        ULONG face_idx;
        CMapClass *ent = CMapView3D_NearestObjectAt(base, &last_pos, &face_idx, 0, nullptr);
        if (ent && face_idx) {
            CMapSolid *solid = CMapClass_AsSolid(ent);
            CMapDoc *doc = GetActiveMapDoc();
            if (solid && doc) {
                CMapFace *face = &solid->Faces.items[face_idx];
                ASSERT(face);

                grab.orig_dist = face->plane.dist;
                grab.ent = solid;
                grab.face = face;

                CHistory_MarkUndoPosition(GetHistory(), CMapDoc_GetSelection(doc), "Extrude Face", false);
                CHistory_Keep(GetHistory(), (CMapClass *)solid);

                void *sheet = GetFaceEditSheet();
                ASSERT(sheet);
                // visualize selected face
                orig_CFaceEditSheet_ClickFace(sheet, solid, (int)face_idx, cfSelect | cfClear, ModeSelect);

                grab.grabbing = true;
            }
        }
    }
}

void extrude_face_close() {
    grab.grabbing = false;
    void *sheet = GetFaceEditSheet();
    ASSERT(sheet);
    orig_CFaceEditSheet_ClickFace(sheet, nullptr, -1, cfClear, -1);
}

typedef void (*ClientToWorld_t)(void *this_, Vec3 *vWorld, const Vec2 *vClient);

void extrude_face_mouse_move_3d(CMapView3D *view, const Vec2 *point) {
    last_pos = *point;

    if (!grab.grabbing || !debounce_should_run(&debounce)) {
        return;
    }

    // TODO: make CMapView structs
    void **map_view_vtable = *(void ***)((void *)view + CMAPVIEW2D_OFFSET_MAPVIEW);
    ClientToWorld_t ClientToWorld = map_view_vtable[MAPVIEW_VTABLE_CLIENTTOWORLD];

    Vec3 world;
    ClientToWorld((void *)view + CMAPVIEW2D_OFFSET_MAPVIEW, &world, point);

    CCamera *camera = get_camera(view);
    Vec3 dir = vec3Normalize(vec3Subtract(world, camera->m_ViewPoint));

    // chatgpt code start
    // line/ray closest point:
    //   ray:  world + t * forward, t >= 0
    //   line: anchor + s * normal
    Vec3 normal = grab.face->plane.normal;
    Vec3 anchor = grab.face->plane.points[0];
    Vec3 r = vec3Subtract(world, anchor);
    float a = vec3DotProduct(dir, dir);
    float b = vec3DotProduct(dir, normal);
    float c = vec3DotProduct(dir, r);
    float e = vec3DotProduct(normal, normal);
    float f = vec3DotProduct(normal, r);
    float denom = (a * e) - (b * b);
    float s = ((a * f) - (b * c)) / denom;
    // chatgpt code end

    Vec3 point2 = vec3Add(anchor, vec3Multiply(normal, s));

    float new_dist = vec3DotProduct(grab.face->plane.normal, point2);
    if (new_dist > grab.orig_dist) {
        grab.face->plane.dist = new_dist;
        CMapSolidMethods.CreateFromPlanes(grab.ent, CREATE_BUILD_PLANE_POINTS | CREATE_ALREADY_HAS_POINTS, nullptr);
        grab.ent->base.vtable->PostUpdate(grab.ent, Notify_Transform);
    }
}

void extrude_face_mouse_move_2d(CMapView2D *view, const Vec2 *point) {
    if (!grab.grabbing || !debounce_should_run(&debounce)) {
        return;
    }

    // TODO: make CMapView structs
    void **map_view_vtable = *(void ***)((void *)view + CMAPVIEW2D_OFFSET_MAPVIEW);
    ClientToWorld_t ClientToWorld = map_view_vtable[MAPVIEW_VTABLE_CLIENTTOWORLD];

    Vec3 world;
    ClientToWorld((void *)view + CMAPVIEW2D_OFFSET_MAPVIEW, &world, point);
    world.z = grab.face->plane.points[0].z;

    float new_dist = vec3DotProduct(grab.face->plane.normal, world);
    if (new_dist > grab.orig_dist) {
        grab.face->plane.dist = new_dist;
        CMapSolidMethods.CreateFromPlanes(grab.ent, CREATE_BUILD_PLANE_POINTS | CREATE_ALREADY_HAS_POINTS, nullptr);
        grab.ent->base.vtable->PostUpdate(grab.ent, Notify_Transform);
    }
}
