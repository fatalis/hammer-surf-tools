#include "triggergen.h"
#include "hooks.h"
#include "hammerfuncs.h"
#include "util.h"

#define TRIGGER_HEIGHT 2.0f

#define CREATE_BUILD_PLANE_POINTS 0x0001

static CMapFace *make_trigger_face(CMapSolid *solid, Vec3 *points, int n_points) {
    CMapFace *face = CSolidFaces_MakeFace(&solid->Faces);
    CMapFaceMethods.CreateFace(face, points, n_points, false);
    CMapFaceMethods.SetTexture(face, "tools/toolstrigger", 0);
    face->atom.r = 255; // TODO: temporary fix. find what normally does this
    face->atom.g = 255;
    face->atom.b = 0;
    // TODO: verify these args
    CMapFaceMethods.InitializeTextureAxes(face, TEXTURE_ALIGN_FACE, INIT_TEXTURE_ALL | INIT_TEXTURE_FORCE);
    return face;
}

CMapEntity *create_trigger(CMapFace *pTargetFace) {
    if (!pTargetFace || pTargetFace->Points.length < 3) {
        log_msg("create_trigger: invalid face\n");
        return nullptr;
    }

    CMapDoc *doc = GetActiveMapDoc();
    ASSERT(doc);

    const int n_points = (int)pTargetFace->Points.length;

    CMapSolid *solid = new_CMapSolid();

    // bottom face (input face's location)
    Vec3 orig[n_points];
    for (auto i = 0; i < n_points; i++) {
        orig[i] = pTargetFace->Points.items[i].point;
    }
    make_trigger_face(solid, orig, -n_points);

    // top (extended along input face's normal)
    ASSERT(vec3Length(pTargetFace->plane.normal) != 0.0f);
    Vec3 normal = vec3Normalize(pTargetFace->plane.normal);
    Vec3 offset = vec3Multiply(normal, TRIGGER_HEIGHT);

    Vec3 top[n_points];
    for (int i = 0; i < n_points; i++) {
        top[i] = vec3Add(pTargetFace->Points.items[i].point, offset);
    }
    make_trigger_face(solid, top, n_points);


    // sides
    for (auto i = 0; i < n_points; i++) {
        auto next = (i + 1) % n_points;

        Vec3 quad[4] = {
            pTargetFace->Points.items[i].point,
            pTargetFace->Points.items[next].point,
            top[next],
            top[i]
        };

        make_trigger_face(solid, quad, ARRAY_LEN(quad));
    }

    CMapEntity *ent = new_CMapEntity();
    CEditGameClass *edit = &ent->m_EditGameClass;

    edit->vtable->SetClass(edit, "trigger_teleport", false);
    edit->vtable->SetKeyValue(edit, "spawnflags", "1"); // clients flag
    ent->base.vtable->AddChild(ent, (CMapClass *)solid);

    ent->base.vtable->CalcBounds(ent, true);
    CMapEntity_SetKVOrigin(ent); // TODO: find out what normally does this when tying to entity

    doc->vtable->AddObjectToWorld(doc, ent, nullptr);

    CHistory_KeepNew(GetHistory(), (CMapClass *)ent, true);

    return ent;
}

void do_trigger_generator() {
    CMapDoc *doc = GetActiveMapDoc();
    void *sheet = GetFaceEditSheet();
    if (!sheet || !doc) {
        return;
    }

    StoredFaceVector *stored_faces = CFaceEditSheet_GetFaces(sheet);

    if (stored_faces->length < 1) {
        AfxMessageBoxF(MB_OK, "Select some faces with the texture tool first.");
        return;
    }

    CHistory_MarkUndoPosition(GetHistory(), CMapDoc_GetSelection(doc), "Trigger Generation", false);

    CMapClass *items[stored_faces->length];
    for (auto i = 0; i < stored_faces->length; i++) {
        StoredFace *sf = &stored_faces->items[i];
        CMapFace *face = sf->pMapFace;
        ASSERT(face);

        CMapEntity *ent = create_trigger(face);
        items[i] = (CMapClass *)ent;
    }

    CMapObjectList list;
    list.items = items;
    list.length = stored_faces->length;
    CSelection_SelectObjectList(doc->m_pSelection, &list, scClear | scSelect);

    CMapDocMethods.SetModifiedFlag(doc, true);

    // TODO: bring up object properties dialog
}
