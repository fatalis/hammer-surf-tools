#ifndef UTIL_H
#define UTIL_H

void SetHInstance(HINSTANCE hInst);
HINSTANCE GetHInstance();

static inline Vec3 BBoxCenter(const BoundingBox *bbox) {
    Vec3 center;
    center.x = (bbox->mins.x + bbox->maxs.x) * 0.5f;
    center.y = (bbox->mins.y + bbox->maxs.y) * 0.5f;
    center.z = (bbox->mins.z + bbox->maxs.z) * 0.5f;
    return center;
}

static inline Vec3 BBoxSize(const BoundingBox *bbox) {
    Vec3 size;
    size.x = bbox->maxs.x - bbox->mins.x;
    size.y = bbox->maxs.y - bbox->mins.y;
    size.z = bbox->maxs.z - bbox->mins.z;
    return size;
}

static inline Vec3 BBoxTrueCenter(CMapClass **ents) {
    float minX = ents[0]->m_Render2DBox.mins.x;
    float minY = ents[0]->m_Render2DBox.mins.y;
    float minZ = ents[0]->m_Render2DBox.mins.z;
    float maxX = ents[0]->m_Render2DBox.maxs.x;
    float maxY = ents[0]->m_Render2DBox.maxs.y;
    float maxZ = ents[0]->m_Render2DBox.maxs.z;

    for (auto i = 1; i < arrlen(ents); i++) {
        if (ents[i]->m_Render2DBox.mins.x < minX) minX = ents[i]->m_Render2DBox.mins.x;
        if (ents[i]->m_Render2DBox.mins.y < minY) minY = ents[i]->m_Render2DBox.mins.y;
        if (ents[i]->m_Render2DBox.mins.z < minZ) minZ = ents[i]->m_Render2DBox.mins.z;

        if (ents[i]->m_Render2DBox.maxs.x > maxX) maxX = ents[i]->m_Render2DBox.maxs.x;
        if (ents[i]->m_Render2DBox.maxs.y > maxY) maxY = ents[i]->m_Render2DBox.maxs.y;
        if (ents[i]->m_Render2DBox.maxs.z > maxZ) maxZ = ents[i]->m_Render2DBox.maxs.z;
    }

    Vec3 center;
    center.x = (minX + maxX) * 0.5f;
    center.y = (minY + maxY) * 0.5f;
    center.z = (minZ + maxZ) * 0.5f;
    return center;
}

static inline bool NormalSurfable(Vec3 *normal) {
    return normal->z < SURF_NORMAL && normal->z >= UNSURF_NORMAL;
}

static inline bool NormalHeadSurfable(Vec3 *normal) {
    return normal->z > -SURF_NORMAL && normal->z <= -UNSURF_NORMAL;
}

static inline float NormalDegrees(Vec3 *normal) {
    const Vec3 up = {{0.0f, 0.0f, 1.0f}};
    return acosf(fabsf(vec3DotProduct(*normal, up))) * (180.0f / (float)M_PI);
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

int AfxMessageBoxF(unsigned int nType, const char* fmt, ...);
CMapEntity *new_CMapEntity();
CMapSolid *new_CMapSolid();
CMapObjectList *CMapDoc_GetSelection(CMapDoc *doc);
void *GetFaceEditSheet();
HWND GetMainWndHwnd();

CMapSolid *CMapClass_AsSolid(CMapClass *ent);
bool CMapClass_IsWorldBrush(CMapClass *ent);
void CMapEntity_SetKVOrigin(CMapEntity *ent);
bool IsAllWorldBrushes(CMapObjectList *selected);

void debug_point(int id, Vec3 *point, uint32_t color);
char *dump_plane(Plane *plane);

typedef struct StoredFace {
    CMapDoc *pMapDoc;
    CMapFace *pMapFace;
    CMapClass *pMapSolid;
} StoredFace;
DEFINE_VECTOR(StoredFace, StoredFaceVector);
StoredFaceVector *CFaceEditSheet_GetFaces(void *sheet);

MAPCLASSTYPE MAPCLASS_TYPE(const char* type_name);

#endif // UTIL_H
