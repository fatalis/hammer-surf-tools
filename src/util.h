#ifndef UTIL_H
#define UTIL_H

#include "common.h"

static inline float Vec3Dot(const Vec3 *a) {
    const Vec3 up = {0, 0, 1};
    return a->x * up.x +
           a->y * up.y +
           a->z * up.z;
}

static inline void BBoxCenter(const BoundingBox *bbox, Vec3 *outCenter) {
    outCenter->x = (bbox->mins.x + bbox->maxs.x) * 0.5f;
    outCenter->y = (bbox->mins.y + bbox->maxs.y) * 0.5f;
    outCenter->z = (bbox->mins.z + bbox->maxs.z) * 0.5f;
}

static inline void BBoxSize(const BoundingBox *bbox, Vec3 *out) {
    out->x = bbox->maxs.x - bbox->mins.x;
    out->y = bbox->maxs.y - bbox->mins.y;
    out->z = bbox->maxs.z - bbox->mins.z;
}

static inline void BBoxTrueCenter(CMapClass *ents, int count, Vec3 *outCenter) {
    if (!ents || count <= 0 || !outCenter) {
        if (outCenter) outCenter->x = outCenter->y = outCenter->z = 0.0f;
        return;
    }

    // initialize min/max with the first entity's render box
    float minX = ents[0].m_Render2DBox.mins.x;
    float minY = ents[0].m_Render2DBox.mins.y;
    float minZ = ents[0].m_Render2DBox.mins.z;
    float maxX = ents[0].m_Render2DBox.maxs.x;
    float maxY = ents[0].m_Render2DBox.maxs.y;
    float maxZ = ents[0].m_Render2DBox.maxs.z;

    // expand to include all remaining bounding boxes
    for (auto i = 1; i < count; ++i) {
        if (ents[i].m_Render2DBox.mins.x < minX) minX = ents[i].m_Render2DBox.mins.x;
        if (ents[i].m_Render2DBox.mins.y < minY) minY = ents[i].m_Render2DBox.mins.y;
        if (ents[i].m_Render2DBox.mins.z < minZ) minZ = ents[i].m_Render2DBox.mins.z;

        if (ents[i].m_Render2DBox.maxs.x > maxX) maxX = ents[i].m_Render2DBox.maxs.x;
        if (ents[i].m_Render2DBox.maxs.y > maxY) maxY = ents[i].m_Render2DBox.maxs.y;
        if (ents[i].m_Render2DBox.maxs.z > maxZ) maxZ = ents[i].m_Render2DBox.maxs.z;
    }

    // compute center of the overall bounding box
    outCenter->x = (minX + maxX) * 0.5f;
    outCenter->y = (minY + maxY) * 0.5f;
    outCenter->z = (minZ + maxZ) * 0.5f;
}

static inline bool Vec3Normalize(Vec3 *v) {
    float len = sqrtf(v->x * v->x + v->y * v->y + v->z * v->z);
    if (len == 0.0f) {
        return false;
    }

    float inv = 1.0f / len;
    v->x *= inv;
    v->y *= inv;
    v->z *= inv;

    return true;
}

static inline void Vec3Mul(Vec3 v, float s, Vec3 *out) {
    out->x = v.x * s;
    out->y = v.y * s;
    out->z = v.z * s;
}

static inline bool NormalSurfable(Vec3 *normal) {
    return normal->z < SURF_NORMAL && normal->z >= UNSURF_NORMAL;
}

static inline bool NormalHeadSurfable(Vec3 *normal) {
    return normal->z > -SURF_NORMAL && normal->z <= -UNSURF_NORMAL;
}

static inline float NormalDegrees(Vec3 *normal) {
    return acosf(fabsf(Vec3Dot(normal))) * (180.0f / (float)M_PI);
}

static inline int SteepnessScore(Vec3 *normal) {
    float znorm = fabsf(normal->z);
    return (int)fmaxf(fminf(floorf((SURF_NORMAL - znorm) * 100.0f), 10.0f), 0.0f);
}

static char surf_string[128];
static inline char *NormalSurfString(Vec3 *normal, const char *pfx, bool *out_surfable) {
    bool surfable = NormalSurfable(normal);
    bool headsurfable = NormalHeadSurfable(normal);
    if (!surfable && !headsurfable) {
        snprintf(surf_string, sizeof(surf_string), "%s is not surfable", pfx);
        *out_surfable = false;
        return surf_string;
    }

    float degrees = NormalDegrees(normal);
    int steepness = SteepnessScore(normal);
    snprintf(surf_string, sizeof(surf_string), "%s is %s: steepness %d/10 (%.1f\xB0)", pfx, headsurfable ? "headsurfable" : "surfable", steepness, (double)degrees);
    *out_surfable = true;
    return surf_string;
}

int AfxMessageBoxF(UINT nType, const char* fmt, ...);
CMapClass *new_CMapEntity();
CMapClass *new_CMapSolid();
RefVector *CMapDoc_GetSelection(CMapDoc *doc);
void *GetFaceEditSheet();

bool CMapClass_IsSolid(CMapClass *ent);
bool CMapClass_IsWorldBrush(CMapClass *ent);
bool IsAllWorldBrushes(RefVector *selected);

typedef struct {
    CMapDoc *pMapDoc;
    CMapFace *pMapFace;
    CMapClass *pMapSolid;
} StoredFace;
RefVector *CFaceEditSheet_GetFaces(void *sheet);

#endif // UTIL_H
