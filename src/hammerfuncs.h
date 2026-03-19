#ifndef HAMMERFUNCS_H
#define HAMMERFUNCS_H

#ifdef USING_AFXMESSAGEBOX
typedef int (*AfxMessageBox_t)(LPCTSTR lpszText, UINT nType, UINT nIDHelp);
extern AfxMessageBox_t AfxMessageBox;
#endif

#ifdef USING_CDIALOG_DOMODAL
typedef int (*CDialog_DoModal_t)(void *this_);
extern CDialog_DoModal_t DoModal;
#endif

#ifdef USING_CHISTORY_KEEPNEW
typedef void (*CHistory_KeepNew_t)(CHistory *this_, CMapClass *pObject, bool bKeepChildren);
extern CHistory_KeepNew_t CHistory_KeepNew;
#endif

#ifdef USING_CHISTORY_KEEP
typedef void (*CHistory_Keep_t)(CHistory *this_, CMapClass *pObject);
extern CHistory_Keep_t CHistory_Keep;
#endif

#ifdef USING_CHISTORY_UNDO
typedef void (*CHistory_Undo_t)(CHistory *this_, CMapObjectList *pNewSelection, CMapObjectList *unk);
extern CHistory_Undo_t CHistory_Undo;
#endif

#ifdef USING_CRENDER_DRAWTEXT
typedef void (*CRender_DrawText_t)(void *this_, const char *text, int x, int y, int nFlags);
extern CRender_DrawText_t CRender_DrawText;
#endif

#ifdef USING_CSOLIDFACES_MAKEFACE
typedef CMapFace *(*CSolidFaces_MakeFace_t)(void *this_);
extern CSolidFaces_MakeFace_t CSolidFaces_MakeFace;
#endif

#ifdef USING_CSTRDLG_CSTRDLG
typedef struct {
    uint8_t unk[CSTRDLG_OFFSET_STRING];
    char *m_string;
    uint8_t padding[CSTRDLG_SIZE - CSTRDLG_OFFSET_STRING - sizeof(char *)];
} CStrDlgInst;
static_assert(offsetof(CStrDlgInst, m_string) == CSTRDLG_OFFSET_STRING, "CMapClass::m_Origin offset wrong");
static_assert(sizeof(CStrDlgInst) == CSTRDLG_SIZE, "CStrDlg size wrong");

typedef void (*CStrDlg_CStrDlg_t)(void *this_, DWORD dwFlags, const char *pszString, const char *pszPrompt, const char *pszTitle);
extern CStrDlg_CStrDlg_t CStrDlg;
#endif

#ifdef USING_CVISGROUP_ISAUTOVISGROUP
typedef bool (*CVisGroup_IsAutoVisGroup_t)(void *this_);
extern CVisGroup_IsAutoVisGroup_t CVisGroup_IsAutoVisGroup;
#endif

#ifdef USING_GETHISTORY
typedef CHistory *(*GetHistory_t)();
extern GetHistory_t GetHistory;
#endif

#ifdef USING_GETMAINWND
typedef void *(*GetMainWnd_t)();
extern GetMainWnd_t GetMainWnd;
#endif

#ifdef USING_MSG
enum MWMSGTYPE {
    mwStatus,
    mwError,
    mwWarning
};

typedef void (*Msg_t)(int type, const char *fmt, ...);
extern Msg_t Msg;
#endif

#ifdef USING_VALVEALLOC
typedef void *(*ValveAlloc_t)(size_t size);
extern ValveAlloc_t ValveAlloc;
#endif

#ifdef USING_WCKEYVALUES_GETVALUE
typedef const char *(*WCKeyValues_GetValue_t)(void *this_, const char *pszKey, int *piIndex);
extern WCKeyValues_GetValue_t WCKeyValues_GetValue;
#endif

#ifdef USING_CSELECTION_SELECTOBJECTLIST
typedef enum {
    scToggle = 0x01,
    scSelect = 0x02,
    scUnselect = 0x04,
    scClear = 0x10,
    scNoLift = 0x20,
    scNoApply = 0x40,
    scCascading = 0x80,
    scCascadingRecursive = 0x100,
    scSelectAll = 0x200,
    scSaveChanges = 0x400
} SelectionCmd;

typedef void (*CSelection_SelectObjectList_t)(void *this_, const CMapObjectList *pList, SelectionCmd cmd);
extern CSelection_SelectObjectList_t CSelection_SelectObjectList;
#endif

#ifdef USING_S_CLASSES
extern MCMSTRUCTVector *S_Classes;
#endif

//
// CMapClass
//

#ifdef USING_CMAPCLASS_ENUMCHILDREN
typedef bool (*EnumChildrenCallback)(CMapClass *ent, void *param);
#endif

typedef struct {
    #ifdef USING_CMAPCLASS_ENUMCHILDREN
    bool (*EnumChildren)(void *this_, EnumChildrenCallback cb, void *param, MAPCLASSTYPE type);
    #endif
    
    #ifdef USING_CMAPCLASS_GETVISGROUPCOUNT
    CVisGroup *(*GetVisGroup)(void *this_, int nIndex);
    #endif

    #ifdef USING_CMAPCLASS_GETVISGROUPCOUNT
    int (*GetVisGroupCount)(void *this_);
    #endif


} CMapClassMethods_t;
extern CMapClassMethods_t CMapClassMethods;
////////////////////////////////


//
// CMapDoc
//

#ifdef USING_CMAPDOC_UPDATEALLVIEWS
#define MAPVIEW_UPDATE_OBJECTS 0x001
#define MAPVIEW_RENDER_NOW     0x800
#endif

typedef struct {

    #ifdef USING_CMAPDOC_UPDATEALLVIEWS
    void (*UpdateAllViews)(void *this_, int nFlags, void *ub);
    #endif

    #ifdef USING_CMAPDOC_SETMODIFIEDFLAG
    void (*SetModifiedFlag)(void *this_, bool bModified);
    #endif

    #ifdef USING_CMAPDOC_DELETEOBJECT
    void (*DeleteObject)(CMapDoc *this_, CMapClass *pObject);
    #endif

} CMapDocMethods_t;
extern CMapDocMethods_t CMapDocMethods;
////////////////////////////////

//
// CMapSolid
//

typedef struct {

    #ifdef USING_CMAPSOLID_ADDPLANE
    bool (*AddPlane)(CMapSolid *this_, const CMapFace *fa);
    #endif

    #ifdef USING_CMAPSOLID_CMAPSOLID
    void (*CMapSolid)(void *this_, CMapClass *parent);
    #endif
    
    #ifdef USING_CMAPSOLID_CLIPBYFACE
    void (*ClipByFace)(CMapClass *this_, const CMapFace *fa, CMapClass **f, CMapClass **b);
    #endif
    
    #ifdef USING_CMAPSOLID_SPLIT
    int (*Split)(CMapSolid *this_, Plane *pPlane, CMapSolid **pFront, CMapSolid **pBack);
    #endif

    #ifdef USING_CMAPSOLID_CREATEFROMPLANES
    bool *(*CreateFromPlanes)(CMapClass *this_, DWORD flags, void *unk);
    #endif

} CMapSolidMethods_t;
extern CMapSolidMethods_t CMapSolidMethods;
////////////////////////////////

//
// CMapFace 
//

#ifdef USING_CMAPFACE_GETORIENTATION
typedef enum {
    FACE_ORIENTATION_FLOOR = 0,
    FACE_ORIENTATION_CEILING,
    FACE_ORIENTATION_NORTH_WALL,
    FACE_ORIENTATION_SOUTH_WALL,
    FACE_ORIENTATION_EAST_WALL,
    FACE_ORIENTATION_WEST_WALL,
    FACE_ORIENTATION_INVALID
} FaceOrientation;

const char *GetFaceOrientationStr(FaceOrientation orientation);
#endif

#ifdef USING_CMAPFACE_INITIALIZETEXTUREAXES
typedef enum {
    TEXTURE_ALIGN_NONE  = 0,
    TEXTURE_ALIGN_WORLD = 1,
    TEXTURE_ALIGN_FACE  = 2,
    TEXTURE_ALIGN_QUAKE = 4
} TextureAlignment;
#define INIT_TEXTURE_FORCE    0x0001
#define INIT_TEXTURE_AXES     0x0002
#define INIT_TEXTURE_ROTATION 0x0004
#define INIT_TEXTURE_SHIFT    0x0008
#define INIT_TEXTURE_SCALE    0x0010
#define INIT_TEXTURE_ALL      (INIT_TEXTURE_AXES | INIT_TEXTURE_ROTATION | INIT_TEXTURE_SHIFT | INIT_TEXTURE_SCALE)
#endif

typedef struct {

    #ifdef USING_CMAPFACE_SETTEXTURE
    void (*SetTexture)(void *this_, const char *pszNewTex, bool bRescaleTextureCoordinates);
    #endif
    
    #ifdef USING_CMAPFACE_INITIALIZETEXTUREAXES
    void (*InitializeTextureAxes)(void *this_, TextureAlignment eAlignment, DWORD dwFlags);
    #endif
    
    #ifdef USING_CMAPFACE_GETORIENTATION
    FaceOrientation (*GetOrientation)(void *this_);
    #endif
    
    #ifdef USING_CMAPFACE_CREATEFACE
    bool (*CreateFace)(CMapFace *this_, Vec3 *points, int npoints, bool bIsCordonFace);
    #endif
    
    #ifdef USING_CMAPFACE_COPYFROM
    CMapFace *(*CopyFrom)(CMapFace *this_, const CMapFace *from, DWORD dwFlags, bool bUpdateDependencies);
    #endif
    
    

} CMapFaceMethods_t;
extern CMapFaceMethods_t  CMapFaceMethods;
////////////////////////////////

//
//  CMapEntity
//

typedef struct {
    #ifdef USING_CMAPENTITY_HASSOLIDCHILDREN
    bool (*HasSolidChildren)(void *this_);
    #endif
    
    #ifdef USING_CMAPENTITY_CMAPENTITY
    void (*CMapEntity)(CMapEntity *this_);
    #endif

} CMapEntityMethods_t;
extern CMapEntityMethods_t CMapEntityMethods;
////////////////////////////////

#endif // HAMMERFUNCS_H
