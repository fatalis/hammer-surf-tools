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
#define CMD_RAMP_GENERATOR        32069 // stay below signed short range
#define CMD_TRIGGER_GENERATOR     32068
#define CMD_ANGLEFIX              32067
#define IDR_POPUPS                  182
#define IDR_FORGEMAPTYPE            129
#define IDD_FACEEDIT                194

typedef const char * MAPCLASSTYPE;

// typedefs
typedef struct CMapClass CMapClass;
typedef struct CMapFace CMapFace;
typedef struct CMapDoc CMapDoc;
typedef vec3 Vec3;
#define VEC3_ONE VEC3(1, 1, 1)

typedef vec2 Vec2;
#define VEC2_ONE VEC2(1, 1)

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


typedef enum {
    AXIS_X,
    AXIS_Y,
    AXIS_Z,
    AXIS_UNK,
} Axis;


typedef struct HAMMER_ALIGN {
    Vec3 normal;
    float dist;
    Vec3 points[3];
} Plane;


typedef enum {
    YAW,
    PITCH,
    ROLL
} Angle;

typedef union {
    struct {
        float yaw;
        float pitch;
        float roll;
    };
    float v[3];
} Euler;
#define EULER_ZERO {{0.0f, 0.0f, 0.0f}}


typedef struct HAMMER_ALIGN {
    void *vtable;
    Vec3 mins;
    Vec3 maxs;
} BoundingBox;


// msvc desctuctor flags, guessed name
typedef enum {
    DELETE_NONE,
    DELETE_OBJ,
    DELETE_ARRAY
} DeleteFlags;

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

typedef struct HAMMER_ALIGN {
    uint8_t padding[CVISGROUP_OFFSET_NAME];
    char m_szName[128];
} CVisGroup; // incomplete sized type
static_assert(offsetof(CVisGroup, m_szName) == CVISGROUP_OFFSET_NAME, "CVisGroup::m_szName offset wrong");
DEFINE_VECTOR(CVisGroup *, CVisGroupList);

typedef void VMatrix;
typedef void CMapWorld;
typedef void CChunkFile;
typedef void CSaveInfo;
typedef void CMapView2D;
typedef void CMapViewLogical;
typedef void HitInfo_t;
typedef void CBaseTool;

typedef bool         (*CMapClass_BoolNoArgs_t)        (void *this_);
typedef MAPCLASSTYPE (*CMapAtom_GetType_t)            (void *this_);
typedef void         (*CMapAtom_SetRenderColor_t)     (void *this_, uint32_t color);
typedef void         (*CMapAtom_SetRenderColor2_t)    (void *this_, uint8_t r, uint8_t g, uint8_t b);
typedef void         (*CMapAtom_SetParent_t)          (void *this_, CMapAtom *parent);
typedef CMapClass   *(*CMapAtom_GetParent_t)          (void *this_);
typedef void         (*CMapAtom_DoTransform_t)        (void *this_, const VMatrix *matrix);
typedef void         (*CMapPoint_SetOrigin_t)         (void *this_, Vec3 *pos);
typedef void         (*Dtor_t)                        (void *this_, DeleteFlags flags);
typedef size_t       (*CMapClass_GetSize_t)           (void *this_);
typedef void         (*CMapClass_AddVisGroup_t)       (void *this_, CVisGroup *visgroup);
typedef bool         (*CMapClass_UpdateObjectColor_t) (void *this_);
typedef void         (*CMapClass_AddChild_t)          (void *this_, CMapClass *child);
typedef void         (*CMapClass_CopyChildrenFrom_t)  (void *this_, CMapClass *from, bool bUpdateDependencies);
typedef void         (*CMapClass_RemoveAllChildren_t) (void *this_);
typedef void         (*CMapClass_RemoveChild_t)       (void *this_, CMapClass *child, bool bUpdateBounds);
typedef void         (*CMapClass_UpdateChild_t)       (void *this_, CMapClass *child);
typedef CMapClass   *(*CMapClass_GetParent_t)         (void *this_);
typedef void         (*CMapClass_SetParent_t)         (void *this_, CMapClass *parent);
typedef void         (*CMapClass_ReplaceTargetName_t) (void *this_, const char *oldName, const char *newName);
typedef void         (*CMapClass_OnAddToWorld_t)      (void *this_, CMapWorld *world);
typedef void         (*CMapClass_OnClone_t)           (void *this_, CMapClass *clone, CMapWorld *world, const void *origList, void *newList);
typedef void         (*CMapClass_OnPreClone_t)        (void *this_, CMapClass *clone, CMapWorld *world, const void *origList, void *newList);
typedef void         (*CMapClass_OnPrePaste_t)        (void *this_, CMapClass *copy, CMapWorld *srcWorld, CMapWorld *dstWorld, const void *origList, void *newList);
typedef void         (*CMapClass_OnPaste_t)           (void *this_, CMapClass *copy, CMapWorld *srcWorld, CMapWorld *dstWorld, const void *origList, void *newList);
typedef void         (*CMapClass_OnParentKeyChanged_t)(void *this_, const char *key, const char *value);
typedef void         (*CMapClass_OnRemoveFromWorld_t) (void *this_, CMapWorld *world, bool bNotifyChildren);
typedef void         (*CMapClass_OnUndoRedo_t)        (void *this_);
typedef bool         (*CMapClass_OnApply_t)           (void *this_);
typedef void         (*CMapClass_CalcBounds_t)        (void *this_, bool bFullUpdate);
typedef void         (*CMapClass_SetLogicalPosition_t)(void *this_, const Vec2 *pos);
typedef const Vec2  *(*CMapClass_GetLogicalPosition_t)(void *this_);
typedef void         (*CMapClass_GetRenderLogicalBox_t)(void *this_, Vec2 *mins, Vec2 *maxs);
typedef CMapClass   *(*CMapClass_PrepareSelection_t)  (void *this_, int mode);
typedef bool         (*CMapClass_IsMapClass_t)        (void *this_, MAPCLASSTYPE type);
typedef bool         (*CMapClass_IsWorld_t)           (void *this_);
typedef CMapClass   *(*CMapClass_Copy_t)              (void *this_, bool bUpdateDependencies);
typedef CMapClass   *(*CMapClass_CopyFrom_t)          (void *this_, CMapClass *from, bool bUpdateDependencies);
typedef bool         (*CMapClass_HitTest2D_t)         (void *this_, CMapView2D *view, const Vec2 *pt, HitInfo_t *hitData);
typedef bool         (*CMapClass_HitTestLogical_t)    (void *this_, CMapViewLogical *view, const Vec2 *pt, HitInfo_t *hitData);
typedef CBaseTool   *(*CMapClass_GetToolObject_t)     (void *this_, int hitData, bool bAttach);
typedef int          (*CMapClass_SaveVMF_t)           (void *this_, CChunkFile *file, CSaveInfo *saveInfo);
typedef int          (*CMapClass_SaveEditorData_t)    (void *this_, CChunkFile *file);
typedef bool         (*CMapClass_ShouldSerialize_t)   (void *this_);
typedef void         (*CMapClass_PostloadWorld_t)     (void *this_, CMapWorld *world);
typedef void         (*CMapClass_PresaveWorld_t)      (void *this_);
typedef bool         (*CMapClass_IsIntersectingCordon_t)(void *this_, const Vec3 *mins, const Vec3 *maxs);
typedef const char  *(*CMapClass_GetDescription_t)      (void *this_);

typedef struct {
    // ==== CMapAtom ====
    CMapAtom_GetType_t             GetType;                    //  0
    void                          *CMapClass_01;               //  1
    void                          *CMapClass_02;               //  2
    void                          *CMapClass_03;               //  3
    CMapAtom_SetRenderColor_t      SetRenderColor;             //  4
    CMapAtom_SetRenderColor2_t     SetRenderColor2;            //  5
    void                          *CMapClass_06;               //  6
    CMapAtom_SetParent_t           _SetParent;                 //  7
    CMapAtom_GetParent_t           _GetParent;                 //  8
    void                          *CMapClass_09;               //  9
    void                          *CMapClass_10;               // 10
    void                          *CMapClass_11;               // 11
    void                          *CMapClass_12;               // 12
    void                          *CMapClass_13;               // 13
    void                          *CMapPoint_PostUpdate;       // 14
    void                          *CMapClass_15;               // 15
    void                          *CMapClass_16;               // 16
    CMapAtom_DoTransform_t         DoTransform;                // 17

    // ==== CMapPoint ====
    CMapPoint_SetOrigin_t          SetOrigin;                  // 18

    // ==== CMapClass ====
    Dtor_t                         Dtor;                       // 19
    CMapClass_GetSize_t            GetSize;                    // 20
    CMapClass_AddVisGroup_t        AddVisGroup;                // 21
    CMapClass_UpdateObjectColor_t  UpdateObjectColor;          // 22
    CMapClass_AddChild_t           AddChild;                   // 23
    CMapClass_CopyChildrenFrom_t   CopyChildrenFrom;           // 24
    CMapClass_RemoveAllChildren_t  RemoveAllChildren;          // 25
    CMapClass_RemoveChild_t        RemoveChild;                // 26
    CMapClass_UpdateChild_t        UpdateChild;                // 27
    CMapClass_GetParent_t          GetParent;                  // 28
    CMapClass_SetParent_t          SetParent;                  // 29
    CMapClass_ReplaceTargetName_t  ReplaceTargetName;          // 30
    CMapClass_OnAddToWorld_t       OnAddToWorld;               // 31
    CMapClass_OnClone_t            OnClone;                    // 32
    CMapClass_OnPreClone_t         OnPreClone;                 // 33
    CMapClass_OnPrePaste_t         OnPrePaste;                 // 34
    CMapClass_OnPaste_t            OnPaste;                    // 35
    void                          *CMapClass_36;               // 36
    void                          *NotifyDependents;           // 37
    CMapClass_OnParentKeyChanged_t OnParentKeyChanged;         // 38
    CMapClass_OnRemoveFromWorld_t  OnRemoveFromWorld;          // 39
    CMapClass_OnUndoRedo_t         OnUndoRedo;                 // 40
    CMapClass_OnApply_t            OnApply;                    // 41
    CMapClass_CalcBounds_t         CalcBounds;                 // 42
    CMapClass_SetLogicalPosition_t SetLogicalPosition;         // 43
    CMapClass_GetLogicalPosition_t GetLogicalPosition;         // 44
    CMapClass_GetRenderLogicalBox_t GetRenderLogicalBox;       // 45
    CMapClass_PrepareSelection_t   PrepareSelection;           // 46
    void                          *PostUpdate;                 // 47
    CMapClass_IsMapClass_t         IsMapClass;                 // 48
    CMapClass_IsWorld_t            IsWorld;                    // 49
    CMapClass_Copy_t               Copy;                       // 50
    CMapClass_CopyFrom_t           CopyFrom;                   // 51
    CMapClass_HitTest2D_t          HitTest2D;                  // 52
    CMapClass_HitTestLogical_t     HitTestLogical;             // 53
    CMapClass_GetToolObject_t      GetToolObject;              // 54
    CMapClass_SaveVMF_t            SaveVMF;                    // 55
    CMapClass_SaveEditorData_t     SaveEditorData;             // 56
    CMapClass_ShouldSerialize_t    ShouldSerialize;            // 57
    CMapClass_PostloadWorld_t      PostloadWorld;              // 58
    CMapClass_PresaveWorld_t       PresaveWorld;               // 59

    CMapClass_BoolNoArgs_t         IsGroup;                    // 60
    CMapClass_BoolNoArgs_t         IsScaleable;                // 61
    CMapClass_BoolNoArgs_t         IsClutter;                  // 62
    CMapClass_BoolNoArgs_t         CanBeCulledByCordon;        // 63
    CMapClass_IsIntersectingCordon_t IsIntersectingCordon;     // 64
    CMapClass_BoolNoArgs_t         IsEditable;                 // 65
    CMapClass_BoolNoArgs_t         ShouldSnapToHalfGrid;       // 66
    CMapClass_BoolNoArgs_t         IsSolid;                    // 67

    void                          *CMapClass_68;               // 68
    void                          *CMapClass_69;               // 69
    void                          *CMapClass_70;               // 70
    CMapClass_GetDescription_t     GetDescription;             // 71
    CMapClass_BoolNoArgs_t         IsLogical;                  // 72
    CMapClass_BoolNoArgs_t         IsVisibleLogical;           // 73
    void                          *CMapClass_74;               // 74
    void                          *CMapClass_75;               // 75
    void                          *CMapClass_76;               // 76
    void                          *CMapClass_77;               // 77
    void                          *CMapClass_78;               // 78
    void                          *CMapClass_79;               // 79
    void                          *CMapClass_80;               // 80
    void                          *CMapClass_81;               // 81
    void                          *CMapClass_82;               // 82

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
} CEditGameClass; // incomplete sized type


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
    BoundingBox m_CullBox;       // 0x068
    BoundingBox m_BoundingBox;   // 0x088
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
    bool m_bVisible2D;           // 0x180
    bool m_bVisible;             // 0x181
    bool m_bVisGroupShown;       // 0x182
    bool m_bVisGroupAutoShown;   // 0x183
    bool m_bTemporary;           // 0x184
    char CMapClass_0x184[3];     // 0x185
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
static_assert(offsetof(CMapSolid, Faces) == CMAPSOLID_OFFSET_FACES, "CMapSolid::Faces offset wrong");
static_assert(sizeof(CMapSolid)          == CMAPSOLID_SIZE,         "CMapSolid size wrong");


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
    // start of TEXTURE struct, rest is unimplemented
    void *pad2;                                                                          // 0x050
    char texture[260]; // texture name MAX_PATH                                          // 0x058
    uint8_t padding[CMAPFACE_OFFSET_PLANE - CMAPATOM_SIZE - 260 - 8];                    // 0x15C
    Plane plane;                                                                         // 0x190
    int pad;                                                                             // 0x1C4
    Vec3PointsVector Points;                                                             // 0x1C8
    uint8_t padding2[CMAPFACE_SIZE - CMAPFACE_OFFSET_POINTS - sizeof(Vec3PointsVector)]; // 0x1E0
} CMapFace; // 0x360
static_assert(offsetof(CMapFace, plane)   == CMAPFACE_OFFSET_PLANE,   "CMapFace::plane offset wrong");
static_assert(offsetof(CMapFace, Points)  == CMAPFACE_OFFSET_POINTS,  "CMapFace::Points offset wrong");
static_assert(offsetof(CMapFace, texture) == CMAPFACE_OFFSET_TEXTURE, "CMapFace::texture offset wrong");
static_assert(sizeof(CMapFace)            == CMAPFACE_SIZE,           "CMapFace size wrong");


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

typedef struct MCMSTRUCT {
    const char* Type;      
    void* (*pfnNew)(void);
} MCMSTRUCT;

DEFINE_VECTOR(MCMSTRUCT, MCMSTRUCTVector);

#endif // COMMON_H
