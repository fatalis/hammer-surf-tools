#include "measure.h"
#include "util.h"
#include "hooks.h"
#include "hammerfuncs.h"

void measure_clipper_plane(void *this_, void *pRender) {
    Vec3 *ptr = (void *)this_ + CLIPPER3D_OFFSET_PLANE_NORMAL;
    Vec3 normal = {{
        fabsf(ptr->x),
        fabsf(ptr->y),
        fabsf(ptr->z)
    }};

    bool surfable;
    char *str = NormalSurfString(&normal, "clipper plane ", &surfable);
    if (surfable) {
        CRender_DrawText(pRender, str, 10, 30, 2);
    }
}

void measure_render_2d(void *this_, void *pRender) {
    CMapDoc *doc = GetActiveMapDoc();
    if (!doc) {
        return;
    }

    CMapObjectList *selected = CMapDoc_GetSelection(doc);

    if (selected->length != 1) {
        return;
    }

    CMapSolid *solid = CMapClass_AsSolid(selected->items[0]);
    if (!solid) {
        return;
    }

    int lines = 1;
    int n_face = 0;
    CMapFace *face;
    FOR_EACH_VEC_PTR(face, &solid->Faces) {
        char buf[64];
        snprintf(buf, sizeof(buf), "face %d ", n_face);
        n_face++;

        bool surfable;
        char *str = NormalSurfString(&face->plane.normal, buf, &surfable);
        if (surfable) {
            CRender_DrawText(pRender, str, 10, 30 * lines, 2);
            lines++;

            if (lines >= 6) {
                CRender_DrawText(pRender, "...", 10, 30 * lines, 2);
                return;
            }
        }
    }
}
