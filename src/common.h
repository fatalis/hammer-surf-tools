#ifndef COMMON_H
#define COMMON_H

#define HOOK_DEBUG 1

#include <stdarg.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#if HOOK_DEBUG
void log_msg(const char *fmt, ...);
#else
#define log_msg(...) ((void)0)
#endif

#define ARRAY_LEN(x) (sizeof(x) / sizeof((x)[0]))

#define MAPVIEW_UPDATE_OBJECTS 0x001
#define MAPVIEW_RENDER_NOW 0x800
// status bar ids
#define SBI_SIZE 3
#define SBI_PROMPT 0

#define SURF_NORMAL 0.7f
// an arbitrary cutoff for a very steep ramp that you can somewhat stay on
#define UNSURF_NORMAL 0.28f

#define OFFSET_OBJECT_PROPERTIES     0x1F8
#define OFFSET_HWND                  0x40
#define CMAPFACE_SIZE                    0x360
#define CMAPENTITY_SIZE                  0x280
#define CMAPSOLID_SIZE                   0x230
#define CLIPPER3D_OFFSET_PLANE_NORMAL 0xFC

// found in Selection3D::RenderTool2D
#define SELECTION3D_OFFSET_SELECTION  0x1A8
#define SELECTION_OFFSET_SEL_LIST     0x18
#define FACEEDITSHEET_OFFSET_FACES    0x2F98 // TODO: probably want to scan for this instead
// #define FACE_OFFSET_ID               0x2B8

#define CMD_CURVED_RAMP_GENERATOR 42069
#define CMD_TRIGGER_GENERATOR     42068
#define CMD_ANGLEFIX              42067

#define CSTRDLG_SIZE 320

#define IDR_POPUPS                      182
#define IDR_FORGEMAPTYPE                129
#define IDD_FACEEDIT                    194

typedef struct CMapClass CMapClass;
typedef struct CMapFace CMapFace;
typedef struct CMapDoc CMapDoc;

typedef struct { // TODO: find real size
    uint8_t unk[320];
    char *str;
    uint8_t unk2[900];
} CStrDlgInst;

typedef struct {
    void **items;
    uint64_t capacity; // guessed
    // int padding;
    int length;
} RefVector;
static_assert(sizeof(RefVector) == 8 + 8 + 4 + 4, "RefVector size wrong");

typedef struct {
    float x;
    float y;
    float z;
} Vec3;

typedef struct {
    Vec3 normal;
    float dist;
    Vec3 points[3];
} Plane;

// TODO: make a RefVector macro
typedef struct {
    CMapFace *list;
    uint64_t *capacity;
    // int padding;
    int length;
} FaceVector;

typedef struct {
    CMapDoc *pMapDoc;
    CMapFace *pMapFace;
    CMapClass *pMapSolid;
} StoredFace;

// TODO: make a RefVector macro
typedef struct {
    StoredFace *list;
    uint64_t *capacity;
    // int padding;
    int length;
} FaceEditSheetFaces;

typedef struct {
    float yaw;
    float pitch;
    float roll;
} Euler;

typedef struct {
    void *vtable;
    Vec3 mins;
    Vec3 maxs;
} BoundingBox;

typedef struct {
    int pos[3];
    CMapClass *ent;
} FindEntity_t;

typedef struct {
    void *unk[8];
    HWND hWnd;
} MainFrame;

typedef struct {
    void *unk[8];
    MainFrame *MainFrame;
} WndPtr;

typedef struct {
    void *vtable;
    WndPtr *m_pMainWnd;
    HINSTANCE hInst;
    // ...there are more useful fields to implement
} CWinApp;

// typedef float mat4[4][4];

#define INIT_TEXTURE_FORCE			0x0001
#define INIT_TEXTURE_AXES			0x0002
#define INIT_TEXTURE_ROTATION		0x0004
#define INIT_TEXTURE_SHIFT			0x0008
#define INIT_TEXTURE_SCALE			0x0010
#define INIT_TEXTURE_ALL			(INIT_TEXTURE_AXES | INIT_TEXTURE_ROTATION | INIT_TEXTURE_SHIFT | INIT_TEXTURE_SCALE)

typedef enum {
    TEXTURE_ALIGN_NONE	= 0x0000,
    TEXTURE_ALIGN_WORLD = 0x0001,
    TEXTURE_ALIGN_FACE	= 0x0002,
    TEXTURE_ALIGN_QUAKE = 0x0004
} TextureAlignment;

typedef enum {
    FACE_ORIENTATION_FLOOR = 0,
    FACE_ORIENTATION_CEILING,
    FACE_ORIENTATION_NORTH_WALL,
    FACE_ORIENTATION_SOUTH_WALL,
    FACE_ORIENTATION_EAST_WALL,
    FACE_ORIENTATION_WEST_WALL,
    FACE_ORIENTATION_INVALID
} FaceOrientation;

// TODO: move these to respective files
typedef char *(*CMapClass_GetType_t)(void *);
typedef int (*SetPaneText_t)(void *this_, int nIndex, char *lpszNewText, int bUpdate);
typedef void *(*SetFocus_t)(void *this_);
typedef void *(*GetMainWnd_t)();
typedef bool (*EnumChildrenCallback)(CMapClass *ent, void *param);
typedef bool (*EnumChildren_t)(void *this_, EnumChildrenCallback cb, void *param, const char *type);
typedef void (*SetActiveMapDoc_t)(void *doc);
typedef void (*SetModifiedFlag_t)(void *this_, bool bModified);
// typedef void (*CMapClass_DoTransform_t)(void *this_, mat4 *mtx);
typedef void (*CMapDoc_UpdateAllViews_t)(void *this_, int nFlags, void *ub);
typedef void (*CFaceEditSheet_ClickFace_t)(void *this_, CMapClass *pSolid, int faceIndex, int cmd, int clickMode);
typedef void (*DrawBoundsText_t)(void *pRender, const float *Mins, const float *Maxs, int nFlags);
typedef void (*CRender_DrawText_t)(void *this_, const char *text, int x, int y, int nFlags);
typedef void (*Clipper3D_DrawBrushExtents_t)(void *this_, void *pRender, void *pSolid, int nFlags);
// typedef void (*CMapSolid_DoTransform_t)(void *this_, const float *matrix);
typedef void (*Selection3D_RenderTool2D_t)(void *this_, void *pRender);
typedef CMapClass *(*CMapClass_Copy_t)(void *this_, bool bUpdateDependencies);
typedef CMapClass *(*CMapClass_CopyFrom_t)(void *this_, CMapClass *from, bool bUpdateDependencies);
typedef void (*CMapClass_CopyChildrenFrom_t)(void *this_, void *from, bool bUpdateDependencies);
typedef void (*CMapDoc_AddObjectToWorld_t)(void *this_, void *obj, void *parent);
typedef void (*CMapClass_CalcBounds_t)(void *this_, bool bFullUpdate);
typedef void (*CStrDlg_CStrDlg_t)(void *this_, DWORD dwFlags, const char *pszString, const char *pszPrompt, const char *pszTitle);
typedef int (*CDialog_DoModal_t)(void *this_);
// typedef CWinApp *(*AfxGetApp_t)(void);
typedef LRESULT (*AfxWndProc_t)(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
typedef void (*CMapFace_SetTexture_t)(void *this_, const char *pszNewTex, bool bRescaleTextureCoordinates);
typedef void (*CMapEntity_CMapEntity_t)(void *this_);
typedef void (*CMapSolid_CMapSolid_t)(void *this_, CMapClass *parent);
typedef void (*CEditGameClass_SetClass_t)(void *this_, const char *pszClassname, bool bLoading);
typedef void (*CEditGameClass_SetKeyValue_t)(void *this_, const char *key, const char *value);
typedef void (*CMapClass_AddChild_t)(void *this_, CMapClass *pChild);
typedef int (*AfxMessageBox_t)(LPCTSTR lpszText, UINT nType, UINT nIDHelp);
typedef void (*Msg_t)(int type, const char *fmt, ...);
typedef bool (*EnableMenuItem_t)(HMENU hMenu, UINT uIDEnableItem, UINT uEnable);
typedef void *(*ValveAlloc_t)(size_t size);
typedef void (*CMapSolid_ClipByFace_t)(CMapClass *this_, const CMapFace *fa, CMapClass **f, CMapClass **b);
typedef bool (*CMapSolid_AddPlane_t)(CMapClass *this_, const CMapFace *fa);
typedef CMapFace *(*CSolidFaces_MakeFace_t)(void *this_);
typedef CMapFace *(*CMapFace_CopyFrom_t)(CMapFace *this_, const CMapFace *from, DWORD dwFlags, bool bUpdateDependencies);
typedef bool *(*CMapSolid_CreateFromPlanes_t)(CMapClass *this_, DWORD flags, void *unk);
typedef bool (*CMapFace_CreateFace_t)(CMapFace *this_, Vec3 *points, int npoints, bool bIsCordonFace);
typedef void (*CMapPoint_SetOrigin_t)(void *this_, Vec3 *pos);
typedef void (*CMapFace_InitializeTextureAxes_t)(void *this_, TextureAlignment eAlignment, DWORD dwFlags);
typedef bool (*CToolMaterial_OnMouseMove3D_t)(void *this_, void *pView, UINT flags, const float *vPoint);
typedef FaceOrientation (*CMapFace_GetOrientation_t)(void *this_);
typedef void (*CHistory_MarkUndoPosition_t)(void *this_, const void* pSelection, const char *pszName, bool);
typedef void (*CHistory_KeepNew_t)(void *this_, CMapClass *pObject, bool bKeepChildren);
typedef void (*CHistory_Keep_t)(void *this_, CMapClass *pObject);
typedef void *(*GetHistory_t)();

// msvc desctuctor flags, guessed name
enum DeleteFlags {
    DELETE_NONE,
    DELETE_OBJ,
    DELETE_ARRAY
};
typedef void (*Dtor_t)(void *this_, int flags);

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

typedef struct {
    CMapClass_GetType_t GetType; // 0

    void *CMapClass_01; void *CMapClass_02; void *CMapClass_03; void *CMapClass_04; void *CMapClass_05;
    void *CMapClass_06;

    void *SetParent;  // 7

                        void *CMapClass_08; void *CMapClass_09; void *CMapClass_10;
    void *CMapClass_11; void *CMapClass_12; void *CMapClass_13; void *CMapClass_14; void *CMapClass_15;
    void *CMapClass_16;

    void *DoTransform; // 17 CMapAtom
    CMapPoint_SetOrigin_t SetOrigin;   // 18 CMapPoint
    Dtor_t Dtor;       // 19 CMapSolid

                                                                                    void *CMapClass_20;
    void *CMapClass_21; void *CMapClass_22;

    CMapClass_AddChild_t AddChild;     // 23
    void *CMapClass_CopyChildrenFrom;  // 24

                                                                                    void *CMapClass_25;
    void *CMapClass_26; void *CMapClass_27; void *CMapClass_28; void *CMapClass_29; void *CMapClass_30;
    void *CMapClass_31; void *CMapClass_32; void *CMapClass_33; void *CMapClass_34; void *CMapClass_35;
    void *CMapClass_36; void *CMapClass_37; void *CMapClass_38; void *CMapClass_39; void *CMapClass_40;
    void *CMapClass_41;

    CMapClass_CalcBounds_t CalcBounds; // 42

                                            void *CMapClass_43; void *CMapClass_44; void *CMapClass_45;
    void *CMapClass_46; void *CMapClass_47; void *CMapClass_48; void *CMapClass_49;

    CMapClass_Copy_t     Copy;         // 50
    void                *CopyFrom;     // 51
} CMapClassVTable;

typedef struct {
    CEditGameClass_SetClass_t SetClass;
    CEditGameClass_SetKeyValue_t SetKeyValue;
} CEditGameClassVTable;

typedef struct {
    CEditGameClassVTable *vtable;
} CEditGameClass;

typedef struct CMapClass {
    CMapClassVTable *vtable;   // 0x00
    void *CMapAtom_0x08;       // 0x08
    void *CMapAtom_0x10;       // 0x10
    void *CMapAtom_0x18;       // 0x18
    void *CMapAtom_0x20;       // 0x20
    void *CMapAtom_0x28;       // 0x28
    void *CMapAtom_0x30;       // 0x30
    void *CMapAtom_0x38;       // 0x38
    void *CMapAtom_0x40;       // 0x40
    void *CMapAtom_0x48;       // 0x48

    Vec3 m_Origin;             // 0x50 CMapPoint
    // 4 bytes implicit padding

    // CMapClass start

    void *CMapClass_0x60;       // 0x60
    void *CMapClass_0x68;       // 0x68
    void *CMapClass_0x70;       // 0x70
    void *CMapClass_0x78;       // 0x78
    void *CMapClass_0x80;       // 0x80
    void *CMapClass_0x88;       // 0x88
    void *CMapClass_0x90;       // 0x90
    void *CMapClass_0x98;       // 0x98
    void *CMapClass_0xA0;       // 0xA0

    BoundingBox m_Render2DBox; // 0xA8 (0x18) CMapClass
    
    // becomes CMapSolid somewhere here..

    void *CMapSolid_0xC8;      // 0xC8
    void *CMapSolid_0xD0;      // 0xD0
    void *CMapSolid_0xD8;      // 0xD8
    void *CMapSolid_0xE0;      // 0xE0
    void *CMapSolid_0xE8;      // 0xE8
    void *CMapSolid_0xF0;      // 0xF0
    void *CMapSolid_0xF8;      // 0xF8
    void *CMapSolid_0x100;     // 0x100
    void *CMapSolid_0x108;     // 0x108
    void *CMapSolid_0x110;     // 0x110
    void *CMapSolid_0x118;     // 0x118
    void *CMapSolid_0x120;     // 0x120
    void *CMapSolid_0x128;     // 0x128
    void *CMapSolid_0x130;     // 0x130
    void *CMapSolid_0x138;     // 0x138
    void *CMapSolid_0x140;     // 0x140
    void *CMapSolid_0x148;     // 0x148
    void *CMapSolid_0x150;     // 0x150
    void *CMapSolid_0x158;     // 0x158
    void *CMapSolid_0x160;     // 0x160
    void *CMapSolid_0x168;     // 0x168
    void *CMapSolid_0x170;     // 0x170
    void *CMapSolid_0x178;     // 0x178
    void *CMapSolid_0x180;     // 0x180
    void *CMapSolid_0x188;     // 0x188
    CEditGameClass m_EditGameClass;      // 0x190 // CMapEntity
    void *CMapSolid_0x198;     // 0x198 EditGameClass data? union for CMapFace needed
    FaceVector Faces;          // 0x1A0
} CMapClass; // incomplete sized type

typedef struct __attribute__((packed, aligned(4))) {
    Vec3 vec;
    void *unk1;
    void *unk2;
    void *unk3;
    void *unk4;
    void *unk5;
    void *unk6;
} Vec3Points;

typedef struct CMapFace {
    void *vtable;              // 0x00
    uint8_t padding[0x188];    // 0x08  CMapAtom stuff.. make struct later
    Plane plane;               // 0x190
    int pad;
    Vec3Points *Points;         // 0x1C8
    uint64_t nPoints;           // 0x1D0
    int padding3;               // 0x1D8
    int nPoints_2;              // 0x1DC
    uint8_t padding2[0x180];    // 0x1E0
} CMapFace; // 0x360
static_assert(offsetof(CMapFace, plane)              == 0x190, "CMapFace::plane offset wrong");
static_assert(offsetof(CMapFace, Points)             == 0x1C8, "CMapFace::Points offset wrong");
static_assert(offsetof(CMapFace, nPoints_2)          == 0x1DC, "CMapFace::nPoints_2 offset wrong");
static_assert(sizeof(CMapFace)               == CMAPFACE_SIZE, "CMapFace size wrong");

#define MAPDOC_VTABLE_ADDOBJECTTOWORLD 71
typedef struct {
    uint8_t padding[sizeof(void *) * (MAPDOC_VTABLE_ADDOBJECTTOWORLD)];
    CMapDoc_AddObjectToWorld_t AddObjectToWorld;
} CMapDocVTable;
static_assert(offsetof(CMapDocVTable, AddObjectToWorld)
    == MAPDOC_VTABLE_ADDOBJECTTOWORLD * sizeof(void *), "CMapDoc::AddObjectToWorld offset wrong");
#undef MAPDOC_VTABLE_ADDOBJECTTOWORLD

#define OFFSET_CMAPDOC_MPWORLD 0x166 * 8
typedef struct CMapDoc {
    CMapDocVTable *vtable;
    uint8_t padding[OFFSET_CMAPDOC_MPWORLD - sizeof(CMapDocVTable *)];
    void *m_pWorld;
} CMapDoc; // incomplete sized type
static_assert(offsetof(CMapDoc, m_pWorld) == OFFSET_CMAPDOC_MPWORLD, "CMapDoc::m_pWorld offset wrong");
#undef OFFSET_CMAPDOC_MPWORLD

static_assert(offsetof(CMapClass, m_Origin)          == 0x050, "CMapClass::m_Origin offset wrong");
static_assert(offsetof(CMapClass, m_Render2DBox)     == 0x0A8, "CMapClass::m_Render2DBox offset wrong");
static_assert(offsetof(CMapClass, Faces)             == 0x1A0, "CMapClass::Faces offset wrong");

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

#endif // COMMON_H
