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

    CMapFace **faces = (CMapFace **)&solid->Faces.items;
    for (auto i = 0; i < solid->Faces.length; i++) {
        CMapFace *face = faces[i];
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
    out->roll = 0.0f;
}

static float move_distance(float theta) {
    theta = fmodf(theta + 2.0f * (float)M_PI, (float)M_PI / 2.0f);
    return (sqrtf(2.0f) * cosf(theta - (float)M_PI_4) - 1.0f) * PLAYER_SIZE / 2.0f;
}

// TODO: fix up verts to match what compiler fixes up

void do_anglefix() {
    CMapDoc *doc = GetActiveMapDoc();
    if (!doc) {
        return;
    }

    RefVector *selected = CMapDoc_GetSelection(doc);
    if (!selected->length) {
        AfxMessageBoxF(MB_OK, "Selection should contain at least 1 item.");
        return;
    }

    if (!IsAllWorldBrushes(selected)) {
        AfxMessageBoxF(MB_OK, "Selection should contain only world brushes.");
        return;
    }

    bool undo_pos_started = false;
    int n_unsurfable = 0;
    int n_unneeded = 0;

    for (auto n_item = 0; n_item < selected->length; n_item++)  {
        CMapClass *item = selected->items[n_item];
        assert(item);

        CMapFace *surfable_face = best_surfable_face(item);
        if (!surfable_face) {
            n_unsurfable++;
            continue;
        }

        Vec3 *normal = &surfable_face->plane.normal;

        Euler euler;
        Vec3 normal_inv = {-normal->x, -normal->y, -normal->z}; // TODO: why needed?
        to_euler(&normal_inv, &euler);

        float theta = euler.yaw;
        float dist = move_distance(theta);

        // does compiler snap to 1u?
        if (dist < 0.25f) {
            n_unneeded++;
            continue;
        }

        if (!undo_pos_started) {
            CHistory_MarkUndoPosition(GetHistory(), nullptr, "Anglefix", false);
            undo_pos_started = true;
        }

        Msg(mwStatus, "anglefix displacing by %g", (double)dist);

        float x = dist * cosf(theta);
        float y = dist * sinf(theta);
        Vec3 displacement = {x, y, 0.0f};

        // copy before mutating original brush
        CHistory_Keep(GetHistory(), item);
        CMapClass *copy = item->vtable->Copy(item, false);

        // change original brush to playerclip

        CMapFace **faces = (CMapFace **)&item->Faces.items;
        for (auto i = 0; i < item->Faces.length; i++) {
            CMapFace *face = faces[i];
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

    if (n_unneeded > 0) {
        AfxMessageBoxF(MB_OK, "Warning: %d selected brushes skipped because rampfix wasn't needed.");
    }

    if (n_unsurfable > 0) {
        AfxMessageBoxF(MB_OK, "Warning: %d selected brushes skipped because there were no surfable faces.");
    }
}
