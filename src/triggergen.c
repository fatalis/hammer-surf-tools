#include "common.h"
#include "triggergen.h"
#include "hooks.h"
#include "scriptfuncs.h"
#include "hammerfuncs.h"
#include "util.h"

#define TRIGGER_HEIGHT 2.0f

#define CREATE_BUILD_PLANE_POINTS               0x0001

// void debug_point() {
//     CMapClass *ent = new_CMapEntity();
//     ent->m_EditGameClass.vtable->SetClass(&ent->m_EditGameClass, "info_target", false);
//     char name[128];
//     snprintf(name, sizeof(name), "vTopPoints[%d] %g %g %g\n", i, (double)pFace->Points[i].vec.x, (double)pFace->Points[i].vec.y, (double)pFace->Points[i].vec.z);
//     ent->m_EditGameClass.vtable->SetKeyValue(&ent->m_EditGameClass, "targetname", name);
//     ent->vtable->SetOrigin(ent, &vTopPoints[i]);
//     doc->vtable->AddObjectToWorld(doc, ent, false);
// }

static CMapFace *make_trigger_face(CMapClass *solid, Vec3 *points, int nPoints) {
    CMapFace *face = CSolidFaces_MakeFace(&solid->Faces);
    CMapFace_CreateFace(face, points, nPoints, false);
    CMapFace_SetTexture(face, "tools/toolstrigger", 0);
    // TODO: verify these args
    CMapFace_InitializeTextureAxes(face, TEXTURE_ALIGN_FACE, INIT_TEXTURE_ALL | INIT_TEXTURE_FORCE);
    return face;
}

CMapClass *CreateTriggerExtrudedFromFace(CMapFace *pTargetFace) {
    if (!pTargetFace || pTargetFace->nPoints < 3) {
        log_msg("CreateTriggerExtrudedFromFace: invalid face\n");
        return nullptr;
    }

    CMapDoc *doc = GetActiveMapDoc();
    assert(doc);

    const int nPoints = (int)pTargetFace->nPoints;

    CMapClass *solid = new_CMapSolid();

    // bottom face (input face's location)
    Vec3 orig[nPoints];
    for (auto i = 0; i < nPoints; i++) {
        orig[i] = pTargetFace->Points[i].vec;
    }
    make_trigger_face(solid, orig, -nPoints);

    // top (extended along input face's normal)
    Vec3 vNormal = pTargetFace->plane.normal;
    Vec3Normalize(&vNormal);

    Vec3 vOffset;
    Vec3Mul(vNormal, TRIGGER_HEIGHT, &vOffset);

    Vec3 top[nPoints];
    for (int i = 0; i < nPoints; i++) {
        top[i] = pTargetFace->Points[i].vec;
        top[i].x += vOffset.x;
        top[i].y += vOffset.y;
        top[i].z += vOffset.z;
    }
    make_trigger_face(solid, top, nPoints);


    // sides
    for (auto i = 0; i < nPoints; i++) {
        auto next = (i + 1) % nPoints;

        Vec3 quad[4] = {
            pTargetFace->Points[i].vec,
            pTargetFace->Points[next].vec,
            top[next],
            top[i]
        };

        make_trigger_face(solid, quad, ARRAY_LEN(quad));
    }

    solid->vtable->CalcBounds(solid, false);

    CMapClass *ent = new_CMapEntity();
    ent->m_EditGameClass.vtable->SetClass(&ent->m_EditGameClass, "trigger_teleport", false);
    ent->m_EditGameClass.vtable->SetKeyValue(&ent->m_EditGameClass, "spawnflags", "1"); // clients flag - needed for trigger_multiple only? forgot
    // TODO: set last destination that was set manually
    // ent->m_EditGameClass.vtable->SetKeyValue(&ent->m_EditGameClass, "target", "blah");
    ent->vtable->AddChild(ent, solid);

    doc->vtable->AddObjectToWorld(doc, ent, nullptr);

    CHistory_KeepNew(GetHistory(), ent, true);

    return ent;
}

void do_trigger_generator() {
    CMapDoc *doc = GetActiveMapDoc();
    void *sheet = GetFaceEditSheet();
    if (!sheet || !doc) {
        return;
    }

    FaceEditSheetFaces *faces = CFaceEditSheet_GetFaces(sheet);

    if (faces->length < 1) {
        AfxMessageBoxF(MB_OK, "Select some faces with the texture tool first.");
        return;
    }

    CHistory_MarkUndoPosition(GetHistory(), nullptr, "Trigger Generation", false);

    for (auto i = 0; i < faces->length; i++) {
        StoredFace *sf = &faces->list[i];
        CMapFace *face = sf->pMapFace;
        assert(face);

        CreateTriggerExtrudedFromFace(face);
    }

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
