#ifndef COMMON_H
#define COMMON_H

// used by CI to name each build/release
// this is the current guaranteed working/recommend version
#define TARGET_HAMMERPLUSPLUS_BUILD "8870"

#include <stdarg.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <assert.h>
#include <float.h>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "offsets.h"

void log_msg(const char *fmt, ...);

#define ARRAY_LEN(x) (sizeof(x) / sizeof((x)[0]))

// minimum surf angle
#define SURF_NORMAL 0.7f
// an arbitrary cutoff for a very steep ramp that you can somewhat stay on
#define UNSURF_NORMAL 0.28f

// menu ids
#define CMD_CURVED_RAMP_GENERATOR 42069
#define CMD_TRIGGER_GENERATOR     42068
#define CMD_ANGLEFIX              42067
#define IDR_POPUPS                  182
#define IDR_FORGEMAPTYPE            129
#define IDD_FACEEDIT                194

// structs
typedef struct CMapClass CMapClass;
typedef struct CMapFace CMapFace;
typedef struct CMapDoc CMapDoc;

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
    float yaw;
    float pitch;
    float roll;
} Euler;

typedef struct {
    void *vtable;
    Vec3 mins;
    Vec3 maxs;
} BoundingBox;

typedef struct { // TODO: find real size
    uint8_t unk[320];
    char *str;
    uint8_t unk2[900];
} CStrDlgInst;

// msvc desctuctor flags, guessed name
typedef enum {
    DELETE_NONE,
    DELETE_OBJ,
    DELETE_ARRAY
} DeleteFlags;
typedef void (*Dtor_t)(void *this_, DeleteFlags flags);

typedef char *(*CMapClass_GetType_t)(void *);
typedef void (*CMapPoint_SetOrigin_t)(void *this_, Vec3 *pos);
typedef CMapClass *(*CMapClass_Copy_t)(void *this_, bool bUpdateDependencies);
typedef CMapClass *(*CMapClass_CopyFrom_t)(void *this_, CMapClass *from, bool bUpdateDependencies);
typedef void (*CMapClass_CopyChildrenFrom_t)(void *this_, void *from, bool bUpdateDependencies);
typedef void (*CMapClass_CalcBounds_t)(void *this_, bool bFullUpdate);
typedef void (*CMapClass_AddChild_t)(void *this_, CMapClass *pChild);
typedef CMapClass *(*CMapAtom_GetParent_t)(void *this_);
typedef struct {
    CMapClass_GetType_t GetType; // 0

    void *CMapClass_01; void *CMapClass_02; void *CMapClass_03; void *CMapClass_04; void *CMapClass_05;
    void *CMapClass_06;

    void *SetParent;  // 7
    CMapAtom_GetParent_t GetParent;  // 8

                                            void *CMapClass_09; void *CMapClass_10;
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

typedef void (*CEditGameClass_SetClass_t)(void *this_, const char *pszClassname, bool bLoading);
typedef void (*CEditGameClass_SetKeyValue_t)(void *this_, const char *key, const char *value);
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
static_assert(offsetof(CMapClass, m_Origin)      == CMAPCLASS_OFFSET_ORIGIN,      "CMapClass::m_Origin offset wrong");
static_assert(offsetof(CMapClass, m_Render2DBox) == CMAPCLASS_OFFSET_RENDER2DBOX, "CMapClass::m_Render2DBox offset wrong");
static_assert(offsetof(CMapClass, Faces)         == CMAPCLASS_OFFSET_FACES,       "CMapClass::Faces offset wrong");

// TODO: cleanup
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

typedef struct {
    void *vtable;              // 0x00
    void *unk1;                // 0x08
    void *unk2;                // 0x10
    RefVector m_SelectionList; // 0x18
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

typedef struct CMapDoc {
    CMapDocVTable *vtable;
    uint8_t padding[CMAPDOC_OFFSET_MPWORLD - sizeof(CMapDocVTable *)];
    void *m_pWorld;
    uint8_t padding2[0x50];
    CSelection *m_pSelection;
} CMapDoc; // incomplete sized type
static_assert(offsetof(CMapDoc, m_pWorld)     ==   CMAPDOC_OFFSET_MPWORLD, "CMapDoc::m_pWorld offset wrong");
static_assert(offsetof(CMapDoc, m_pSelection) == CMAPDOC_OFFSET_SELECTION, "CMapDoc::m_pSelection offset wrong");

#endif // COMMON_H
