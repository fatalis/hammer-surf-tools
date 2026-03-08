#include "common.h"
#include "anglefix.h"
#include "hooks.h"
#include "scriptfuncs.h"
#include "hooks.h"
#include "hammerfuncs.h"
#include "util.h"

#define PLAYER_SIZE 32.0f

// credit to not-a-zombie for his code on https://not-a-zombie.github.io/vmf-resizer/

static CMapFace *best_surfable_face(CMapClass *solid) {
    // find best surfable face
    CMapFace *best = nullptr;
    float best_normal_delta;
    const float ideal_normal = 0.64f;

    for (auto i = 0; i < solid->Faces.length; i++) {
        CMapFace *face = &solid->Faces.list[i];
        float znorm = fabsf(face->plane.normal.z);
        float delta = fabsf(znorm - ideal_normal);
        if ((!best || delta < best_normal_delta) && znorm < SURF_NORMAL && znorm > 0.0f) {
            best = face;
            best_normal_delta = delta;
        }
    }

    return best;
}

static void to_euler(Vec3 *v, Euler *out) {
    out->yaw = atan2f(v->y, v->x);
    out->pitch = atan2f(-v->z, sqrtf(v->x * v->x + v->y * v->y));
    out->roll = 0.0;
}

static float move_distance(float theta) {
    theta = fmodf(theta + 2.0f * (float)M_PI, (float)M_PI / 2.0f);
    return (sqrtf(2.0f) * cosf(theta - (float)M_PI_4) - 1.0f) * PLAYER_SIZE / 2.0f;
}

void do_anglefix() {
    CMapDoc *doc = GetActiveMapDoc();
    if (!doc) {
        return;
    }

    RefVector *selected = CMapDoc_GetSelection(doc);

    if (selected->length != 1) {
        AfxMessageBoxF(MB_OK, "Selection should contain exactly 1 solid.");
        return;
    }

    CMapClass *item = selected->items[0];
    assert(item);
    char *name = item->vtable->GetType(item);

    if (strcmp(name, "CMapSolid") != 0) {
        AfxMessageBoxF(MB_OK, "Selection should be a solid. Don't select by Group or Object.");
        return;
    }

    // SetParent null?

    CMapFace *surfable_face = best_surfable_face(item);
    if (!surfable_face) {
        AfxMessageBoxF(MB_OK, "Selected solid had no surfable faces.");
        return;
    }

    CHistory_MarkUndoPosition(GetHistory(), nullptr, "Anglefix", false);

    Vec3 *normal = &surfable_face->plane.normal;

    Euler euler;
    Vec3 normal_inv = {-normal->x, -normal->y, -normal->z}; // TODO: why needed?
    to_euler(&normal_inv, &euler);

    float theta = euler.yaw;
    float dist = move_distance(theta);

    // does compiler snap to 1u?
    if (dist < 0.25f) {
        AfxMessageBoxF(MB_OK, "Anglefix is not needed for this ramp.");
        // copy->vtable->Dtor(copy, DELETE_OBJ);
        return;
    }

    Msg(mwStatus, "anglefix displacing by %g", (double)dist);

    float x = dist * cosf(theta);
    float y = dist * sinf(theta);
    Vec3 displacement = {x, y, 0.0f};

    // copy before mutating original brush
    CHistory_Keep(GetHistory(), item);
    CMapClass *copy = item->vtable->Copy(item, false);

    // change original brush to playerclip
    for (auto i = 0; i < item->Faces.length; i++) {
        CMapFace *face = &item->Faces.list[i];
        CMapFace_SetTexture(face, "tools/toolsplayerclip", false);
        CMapFace_InitializeTextureAxes(face, TEXTURE_ALIGN_FACE, INIT_TEXTURE_ALL | INIT_TEXTURE_FORCE);
    }

    // displace original brush
    TransMove(surfable_face, &displacement);

    CMapClass *ent = new_CMapEntity();
    // TODO: detect if fgd has func_detail_illusionary
    ent->m_EditGameClass.vtable->SetClass(&ent->m_EditGameClass, "func_detail_illusionary", false);
    ent->vtable->AddChild(ent, copy);

    doc->vtable->AddObjectToWorld(doc, ent, nullptr);
    CHistory_KeepNew(GetHistory(), ent, true);
}
