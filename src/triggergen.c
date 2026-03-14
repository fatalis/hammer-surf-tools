#include "triggergen.h"
#include "hooks.h"
#include "hammerfuncs.h"
#include "util.h"

#define TRIGGER_HEIGHT 2.0f

#define CREATE_BUILD_PLANE_POINTS               0x0001

// void debug_point() {
//     CMapClass *ent = new_CMapEntity();
//     ent->m_EditGameClass.vtable->SetClass(&ent->m_EditGameClass, "info_target", false);
//     char name[128];
//     snprintf(name, sizeof(name), "vTopPoints[%d] %g %g %g\n", i, (double)pFace->Points[i].point.x, (double)pFace->Points[i].point.y, (double)pFace->Points[i].point.z);
//     ent->m_EditGameClass.vtable->SetKeyValue(&ent->m_EditGameClass, "targetname", name);
//     ent->vtable->SetOrigin(ent, &vTopPoints[i]);
//     doc->vtable->AddObjectToWorld(doc, ent, false);
// }

static CMapFace *make_trigger_face(CMapSolid *solid, Vec3 *points, int nPoints) {
    CMapFace *face = CSolidFaces_MakeFace(&solid->Faces);
    CMapFace_CreateFace(face, points, nPoints, false);
    CMapFace_SetTexture(face, "tools/toolstrigger", 0);
    face->atom.r = 255; // TODO: temporary fix. find what normally does this
    face->atom.g = 255;
    face->atom.b = 0;
    // TODO: verify these args
    CMapFace_InitializeTextureAxes(face, TEXTURE_ALIGN_FACE, INIT_TEXTURE_ALL | INIT_TEXTURE_FORCE);
    return face;
}

CMapEntity *create_trigger(CMapFace *pTargetFace) {
    if (!pTargetFace || pTargetFace->Points.length < 3) {
        log_msg("create_trigger: invalid face\n");
        return nullptr;
    }

    CMapDoc *doc = GetActiveMapDoc();
    ASSERT(doc);

    const int nPoints = (int)pTargetFace->Points.length;

    CMapSolid *solid = new_CMapSolid();

    // bottom face (input face's location)
    Vec3 orig[nPoints];
    for (auto i = 0; i < nPoints; i++) {
        orig[i] = pTargetFace->Points.items[i].point;
    }
    make_trigger_face(solid, orig, -nPoints);

    // top (extended along input face's normal)
    Vec3 vNormal = pTargetFace->plane.normal;
    Vec3Normalize(&vNormal);

    Vec3 vOffset;
    Vec3Mul(vNormal, TRIGGER_HEIGHT, &vOffset);

    Vec3 top[nPoints];
    for (int i = 0; i < nPoints; i++) {
        top[i] = pTargetFace->Points.items[i].point;
        top[i].x += vOffset.x;
        top[i].y += vOffset.y;
        top[i].z += vOffset.z;
    }
    make_trigger_face(solid, top, nPoints);


    // sides
    for (auto i = 0; i < nPoints; i++) {
        auto next = (i + 1) % nPoints;

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

    CMapDoc_SetModifiedFlag(doc, true);

    // TODO: bring up object properties dialog

    // old scale code, left here for example for TransScale

    // BoundingBox *bbox = &copy->m_Render2DBox;
    // Vec3 size;
    // BBoxSize(bbox, &size);
    //
    // Vec3 delta = {0.0f, 0.0f, size.z};
    // TransMove(copy, &delta);
    //
    // float factor = TRIGGER_HEIGHT / size.z;
    // Vec3 ref = {bbox->maxs.x, bbox->maxs.y, bbox->mins.z};
    // Vec3 scale = {1.0f, 1.0f, factor};
    // TransScale(copy, &ref, &scale);
}
