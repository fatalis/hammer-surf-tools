#ifndef COMMON_H
#define COMMON_H

// used by CI to name each build/release
// this is the current guaranteed working/recommend version
#define TARGET_HAMMERPLUSPLUS_BUILD "8870"

#include "offsets.h"

void log_msg(const char *fmt, ...);

#define HAMMER_ALIGN __attribute__((packed, aligned(4)))
#define ARRAY_LEN(x) (sizeof(x) / sizeof((x)[0]))
#define ASSERT(x) \
    do { if(!(x)) { \
        MessageBoxA(NULL, #x, "Assertion Failed", MB_ICONERROR); \
        DebugBreak(); \
    }} while(0)

// minimum surf angle
#define SURF_NORMAL 0.7f
// an arbitrary cutoff for a very steep ramp that you can somewhat stay on
#define UNSURF_NORMAL 0.28f

#define KEYVALUE_MAX_KEY_LENGTH    80
#define KEYVALUE_MAX_VALUE_LENGTH 512

// menu ids
#define CMD_CURVED_RAMP_GENERATOR 32069 // stay below signed short range
#define CMD_TRIGGER_GENERATOR     32068
#define CMD_ANGLEFIX              32067
#define IDR_POPUPS                  182
#define IDR_FORGEMAPTYPE            129
#define IDD_FACEEDIT                194

// structs
typedef struct CMapClass CMapClass;
typedef struct CMapFace CMapFace;
typedef struct CMapDoc CMapDoc;

// CUtlVector
#define DEFINE_VECTOR(T, Name) \
typedef struct HAMMER_ALIGN { \
    T *items; \
    int allocation_count; \
    int grow_size; \
    int length; \
    int padding; \
} Name

DEFINE_VECTOR(CMapFace, CSolidFaces);
DEFINE_VECTOR(CMapClass *, CMapObjectList);
static_assert(sizeof(CMapObjectList) == 8 + 4 + 4 + 4 + 4,   "CMapObjectList size wrong");
static_assert(offsetof(CMapObjectList, length) == 8 + 4 + 4, "CMapObjectList::length offset wrong");

typedef struct {
    float x;
    float y;
    float z;
} Vec3;

typedef struct HAMMER_ALIGN {
    Vec3 normal;
    float dist;
    Vec3 points[3];
} Plane;

typedef struct {
    float yaw;
    float pitch;
    float roll;
} Euler;

typedef struct HAMMER_ALIGN {
    void *vtable;
    Vec3 mins;
    Vec3 maxs;
} BoundingBox;

typedef struct {
    uint8_t unk[CSTRDLG_OFFSET_STRING];
    char *m_string;
    uint8_t padding[CSTRDLG_SIZE - CSTRDLG_OFFSET_STRING - sizeof(char *)];
} CStrDlgInst;
static_assert(offsetof(CStrDlgInst, m_string) == CSTRDLG_OFFSET_STRING, "CMapClass::m_Origin offset wrong");
static_assert(sizeof(CStrDlgInst) == CSTRDLG_SIZE, "CStrDlg size wrong");

// msvc desctuctor flags, guessed name
typedef enum {
    DELETE_NONE,
    DELETE_OBJ,
    DELETE_ARRAY
} DeleteFlags;
typedef void (*Dtor_t)(void *this_, DeleteFlags flags);

typedef char *(*CMapAtom_GetType_t)(void *);
typedef void (*CMapPoint_SetOrigin_t)(void *this_, Vec3 *pos);
typedef CMapClass *(*CMapClass_Copy_t)(void *this_, bool bUpdateDependencies);
typedef CMapClass *(*CMapClass_CopyFrom_t)(void *this_, CMapClass *from, bool bUpdateDependencies);
typedef void (*CMapClass_CopyChildrenFrom_t)(void *this_, void *from, bool bUpdateDependencies);
typedef void (*CMapClass_CalcBounds_t)(void *this_, bool bFullUpdate);
typedef void (*CMapClass_AddChild_t)(void *this_, CMapClass *pChild);
typedef CMapClass *(*CMapAtom_GetParent_t)(void *this_);
typedef void (*CMapAtom_SetRenderColor_t)(void *this_, uint32_t color);
typedef void (*CMapAtom_SetRenderColor2_t)(void *this_, uint8_t r, uint8_t g, uint8_t b);
typedef bool (*CMapClass_UpdateObjectColor_t)(void *this_);
typedef struct {
    CMapAtom_GetType_t GetType; // 0

    void *CMapClass_01; void *CMapClass_02; void *CMapClass_03;

    CMapAtom_SetRenderColor_t SetRenderColor;   // 4
    CMapAtom_SetRenderColor2_t SetRenderColor2; // 5

    void *CMapClass_06;

    void *SetParent;  // 7
    CMapAtom_GetParent_t GetParent;  // 8

                                            void *CMapClass_09; void *CMapClass_10;
    void *CMapClass_11; void *CMapClass_12; void *CMapClass_13; void *CMapClass_14; void *CMapClass_15;
    void *CMapClass_16;

    void *DoTransform; // 17 CMapAtom
    CMapPoint_SetOrigin_t SetOrigin;   // 18 CMapPoint (GetOrigin is compiled away)
    Dtor_t Dtor;       // 19 CMapSolid

                                                                                    void *CMapClass_20;
    void *CMapClass_21;

    CMapClass_UpdateObjectColor_t UpdateObjectColor; // 22

    CMapClass_AddChild_t AddChild;     // 23
    void *CMapClass_CopyChildrenFrom;  // 24

                                                                                    void *CMapClass_25;
    void *CMapClass_26; void *CMapClass_27;

    void *GetParent2;    // CMapClass version? 28
                                                                void *CMapClass_29; void *CMapClass_30;
    void *CMapClass_31; void *CMapClass_32; void *CMapClass_33; void *CMapClass_34; void *CMapClass_35;
    void *CMapClass_36; void *CMapClass_37; void *CMapClass_38; void *CMapClass_39; void *CMapClass_40;
    void *CMapClass_41;

    CMapClass_CalcBounds_t CalcBounds; // 42

                                            void *CMapClass_43; void *CMapClass_44; void *CMapClass_45;
    void *CMapClass_46; void *CMapClass_47; void *CMapClass_48; void *CMapClass_49;

    CMapClass_Copy_t     Copy;         // 50
    CMapClass_CopyFrom_t *CopyFrom;    // 51
} CMapClassVTable;

// in hammer++ there is no SetClass on CMapEntity
typedef void (*CEditGameClass_SetClass_t)(void *this_, const char *pszClassname, bool bLoading);
typedef void (*CEditGameClass_SetKeyValue_t)(void *this_, const char *key, const char *value);
typedef struct {
    CEditGameClass_SetClass_t SetClass;
    CEditGameClass_SetKeyValue_t SetKeyValue;
} CEditGameClassVTable;

typedef struct {
    CEditGameClassVTable *vtable;
    void *m_KeyValues; // WCKeyValues instance start (not pointer to)
    // need a union in CMapClass if this is defined further
} CEditGameClass; // incomplete sized type

typedef struct HAMMER_ALIGN {
    uint8_t padding[CVISGROUP_OFFSET_NAME];
    char m_szName[128];
} CVisGroup; // incomplete sized type
static_assert(offsetof(CVisGroup, m_szName) == CVISGROUP_OFFSET_NAME, "CVisGroup::m_szName offset wrong");
DEFINE_VECTOR(CVisGroup *, CVisGroupList);

typedef struct HAMMER_ALIGN CMapAtom {
    // vtable                    // 0x00
    void *CMapAtom_0x08;         // 0x08 somewhere here: m_pParent, m_eSelectionState
    void *CMapAtom_0x10;         // 0x10
    void *CMapAtom_0x18;         // 0x18
    void *CMapAtom_0x20;         // 0x20
    void *CMapAtom_0x28;         // 0x28
    void *CMapAtom_0x30;         // 0x30
    void *CMapAtom_0x38;         // 0x38
    void *CMapAtom_0x40;         // 0x40
    int padding0;                // 0x48
    uint8_t r;                   // 0x4C
    uint8_t g;                   // 0x4D
    uint8_t b;                   // 0x4E
    uint8_t padding1;            // 0x4F
} CMapAtom;
static_assert(sizeof(CMapAtom) == CMAPATOM_SIZE - sizeof(void *), "CMapAtom size wrong"); // subtracts vtable ptr size

typedef struct HAMMER_ALIGN CMapPoint {
    Vec3 m_Origin;               // 0x50
    int padding;                 // 0x5C
} CMapPoint;
static_assert(sizeof(CMapPoint) == CMAPPOINT_SIZE, "CMapPoint size wrong");

typedef struct HAMMER_ALIGN CMapClass {
    CMapClassVTable *vtable;     // 0x000
    CMapAtom atom;               // 0x008
    CMapPoint point;

    void *CMapClass_0x60;        // 0x060
    void *CMapClass_0x68;        // 0x068
    void *CMapClass_0x70;        // 0x070
    void *CMapClass_0x78;        // 0x078
    void *CMapClass_0x80;        // 0x080
    void *CMapClass_0x88;        // 0x088
    void *CMapClass_0x90;        // 0x090
    void *CMapClass_0x98;        // 0x098
    void *CMapClass_0xA0;        // 0x0A0

    BoundingBox m_Render2DBox;   // 0x0A8

    void *CMapClass_0xC8;        // 0x0C8
    void *CMapClass_0xD0;        // 0x0D0
    void *CMapClass_0xD8;        // 0x0D8
    void *CMapClass_0xE0;        // 0x0E0
    void *CMapClass_0xE8;        // 0x0E8
    void *CMapClass_0xF0;        // 0x0F0
    void *CMapClass_0xF8;        // 0x0F8
    CMapObjectList m_Children;   // 0x100
    CMapObjectList m_Dependents; // 0x118
    void *CMapClass_0x130;       // 0x130
    void *CMapClass_0x138;       // 0x138
    int m_nID;                   // 0x140 object id
    int m_nRenderFrame;          // 0x144 or this is PrevFrame and NextFrame is current
    int m_nRenderNextFrame;      // 0x148
    int padding2;                // 0x14C
    CVisGroupList m_VisGroups;   // 0x150
    void *CMapClass_0x168;       // 0x168
    void *CMapClass_0x170;       // 0x170
    void *CMapClass_0x178;       // 0x178
    void *CMapClass_0x180;       // 0x180
    void *CMapClass_0x188;       // 0x188
} CMapClass;
static_assert(sizeof(CMapClass) == CMAPCLASS_SIZE, "CMapClass size wrong");

typedef struct HAMMER_ALIGN CMapEntity {
    CMapClass base;                 // 0x000
    CEditGameClass m_EditGameClass; // 0x190
    uint8_t padding[CMAPENTITY_SIZE - CMAPENTITY_OFFSET_EDITGAMECLASS - sizeof(CEditGameClass)];
} CMapEntity;
static_assert(offsetof(CMapEntity, base.point.m_Origin) == CMAPCLASS_OFFSET_ORIGIN,         "CMapClass::m_Origin offset wrong");
static_assert(offsetof(CMapEntity, base.m_Render2DBox)  == CMAPCLASS_OFFSET_RENDER2DBOX,    "CMapClass::m_Render2DBox offset wrong");
static_assert(offsetof(CMapEntity, m_EditGameClass)     == CMAPENTITY_OFFSET_EDITGAMECLASS, "CMapEntity::m_EditGameClass offset wrong");
static_assert(sizeof(CMapEntity)                        == CMAPENTITY_SIZE,                 "CMapEntity size wrong");

typedef struct HAMMER_ALIGN CMapSolid {
    CMapClass base;         // 0x000
    void *CMapSolid_0x190;  // 0x190
    void *CMapSolid_0x198;  // 0x198
    CSolidFaces Faces;      // 0x1A0
    uint8_t padding[CMAPSOLID_SIZE - CMAPSOLID_OFFSET_FACES - sizeof(CSolidFaces)];
} CMapSolid;
static_assert(offsetof(CMapSolid, Faces) == CMAPSOLID_OFFSET_FACES, "CMapClass::Faces offset wrong");
static_assert(sizeof(CMapSolid)          == CMAPSOLID_SIZE,         "CMapClass size wrong");

// changed in hammer++, used to be just a vec3
typedef struct HAMMER_ALIGN {
    Vec3 point;
    int padding;
    uint8_t padding2[VEC3POINTS_SIZE - sizeof(Vec3) - sizeof(int)];
} Vec3Points;
static_assert(sizeof(Vec3Points) == VEC3POINTS_SIZE, "Vec3Points size wrong");
DEFINE_VECTOR(Vec3Points, Vec3PointsVector);

typedef struct HAMMER_ALIGN CMapFace {
    CMapClassVTable *vtable;                                                             // 0x000
    CMapAtom atom;                                                                       // 0x008
    uint8_t padding[CMAPFACE_OFFSET_PLANE - CMAPATOM_SIZE];                              // 0x058
    Plane plane;                                                                         // 0x190
    int pad;                                                                             // 0x1C4
    Vec3PointsVector Points;                                                             // 0x1C8
    uint8_t padding2[CMAPFACE_SIZE - CMAPFACE_OFFSET_POINTS - sizeof(Vec3PointsVector)]; // 0x1E0
} CMapFace; // 0x360
static_assert(offsetof(CMapFace, plane)  == CMAPFACE_OFFSET_PLANE,  "CMapFace::plane offset wrong");
static_assert(offsetof(CMapFace, Points) == CMAPFACE_OFFSET_POINTS, "CMapFace::Points offset wrong");
static_assert(sizeof(CMapFace)           == CMAPFACE_SIZE,          "CMapFace size wrong");

typedef struct HAMMER_ALIGN {
    void *vtable;                   // 0x00
    void *unk1;                     // 0x08
    void *unk2;                     // 0x10
    CMapObjectList m_SelectionList; // 0x18
} CSelection; // incomplete sized type
static_assert(offsetof(CSelection, m_SelectionList) == CSELECTION_OFFSET_SELECTIONLIST, "CSelection::m_SelectionList offset wrong");

#define MAPDOC_VTABLE_ADDOBJECTTOWORLD 71
typedef void (*CMapDoc_AddObjectToWorld_t)(void *this_, void *obj, void *parent);
typedef struct {
    void *padding[MAPDOC_VTABLE_ADDOBJECTTOWORLD];
    CMapDoc_AddObjectToWorld_t AddObjectToWorld;
} CMapDocVTable;
static_assert(offsetof(CMapDocVTable, AddObjectToWorld)
    == MAPDOC_VTABLE_ADDOBJECTTOWORLD * sizeof(void *), "CMapDocVTable::AddObjectToWorld offset wrong");

typedef struct HAMMER_ALIGN CMapDoc {
    CMapDocVTable *vtable;
    uint8_t padding[CMAPDOC_OFFSET_MPWORLD - sizeof(CMapDocVTable *)];
    void *m_pWorld;
    uint8_t padding2[0x50];
    CSelection *m_pSelection;
} CMapDoc; // incomplete sized type
static_assert(offsetof(CMapDoc, m_pWorld)     ==   CMAPDOC_OFFSET_MPWORLD, "CMapDoc::m_pWorld offset wrong");
static_assert(offsetof(CMapDoc, m_pSelection) == CMAPDOC_OFFSET_SELECTION, "CMapDoc::m_pSelection offset wrong");

typedef struct HAMMER_ALIGN {
    uint8_t padding[CHISTORYTRACK_OFFSET_NAME];
    char szName[128];
} CHistoryTrack; // incomplete sized type
static_assert(offsetof(CHistoryTrack, szName) ==   CHISTORYTRACK_OFFSET_NAME, "CHistoryTrack::szName offset wrong");

DEFINE_VECTOR(CHistoryTrack *, HistoryTrackRefVector);
typedef struct HAMMER_ALIGN {
    CHistoryTrack *CurTrack;
    HistoryTrackRefVector Tracks;
} CHistory; // incomplete sized type

#endif // COMMON_H
