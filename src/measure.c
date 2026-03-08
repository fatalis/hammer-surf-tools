#include <assert.h>

#include "common.h"
#include "measure.h"
#include "scriptfuncs.h"
#include "hammerfuncs.h"

void measure_clipper_plane(void *this_, void *pRender) {
    Vec3 *ptr = (void *)this_ + CLIPPER3D_OFFSET_PLANE_NORMAL;
    Vec3 normal = (Vec3){
        fabsf(ptr->x),
        fabsf(ptr->y),
        fabsf(ptr->z)
    };

    bool surfable;
    char *str = NormalSurfString(&normal, "clipper plane ", &surfable);
    if (surfable) {
        CRender_DrawText(pRender, str, 10, 30, 2);
    }
}

void measure_render_2d(void *this_, void *pRender) {
    void *selection = *(void **)((void *)this_ + SELECTION3D_OFFSET_SELECTION);
    RefVector *selected = ((void *)selection + SELECTION_OFFSET_SEL_LIST);

    if (selected->length != 1) {
        return;
    }

    CMapClass *item = selected->items[0];
    assert(item);
    char *name = item->vtable->GetType(item);
    if (strcmp(name, "CMapSolid") != 0) {
        return;
    }

    int lines = 1;
    auto n = item->Faces.length;
    for (auto face_idx = 0; face_idx < n; face_idx++) {
        CMapFace *face = &item->Faces.list[face_idx];

        char buf[64];
        snprintf(buf, sizeof(buf), "face %d ", face_idx);

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
