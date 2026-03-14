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

#ifdef USING_CMAPDOC_UPDATEALLVIEWS
#define MAPVIEW_UPDATE_OBJECTS 0x001
#define MAPVIEW_RENDER_NOW     0x800

typedef void (*CMapDoc_UpdateAllViews_t)(void *this_, int nFlags, void *ub);
extern CMapDoc_UpdateAllViews_t CMapDoc_UpdateAllViews;
#endif

#ifdef USING_CMAPENTITY_CMAPENTITY
typedef void (*CMapEntity_CMapEntity_t)(CMapEntity *this_);
extern CMapEntity_CMapEntity_t CMapEntity_CMapEntity;
#endif

#ifdef USING_CMAPFACE_COPYFROM
typedef CMapFace *(*CMapFace_CopyFrom_t)(CMapFace *this_, const CMapFace *from, DWORD dwFlags, bool bUpdateDependencies);
extern CMapFace_CopyFrom_t CMapFace_CopyFrom;
#endif

#ifdef USING_CMAPFACE_CREATEFACE
typedef bool (*CMapFace_CreateFace_t)(CMapFace *this_, Vec3 *points, int npoints, bool bIsCordonFace);
extern CMapFace_CreateFace_t CMapFace_CreateFace;
#endif

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

typedef FaceOrientation (*CMapFace_GetOrientation_t)(void *this_);
extern CMapFace_GetOrientation_t CMapFace_GetOrientation;
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

typedef void (*CMapFace_InitializeTextureAxes_t)(void *this_, TextureAlignment eAlignment, DWORD dwFlags);
extern CMapFace_InitializeTextureAxes_t CMapFace_InitializeTextureAxes;
#endif

#ifdef USING_CMAPFACE_SETTEXTURE
typedef void (*CMapFace_SetTexture_t)(void *this_, const char *pszNewTex, bool bRescaleTextureCoordinates);
extern CMapFace_SetTexture_t CMapFace_SetTexture;
#endif

#ifdef USING_CMAPSOLID_ADDPLANE
typedef bool (*CMapSolid_AddPlane_t)(CMapSolid *this_, const CMapFace *fa);
extern CMapSolid_AddPlane_t CMapSolid_AddPlane;
#endif

#ifdef USING_CMAPSOLID_CMAPSOLID
typedef void (*CMapSolid_CMapSolid_t)(void *this_, CMapClass *parent);
extern CMapSolid_CMapSolid_t CMapSolid_CMapSolid;
#endif

#ifdef USING_CMAPSOLID_CLIPBYFACE
typedef void (*CMapSolid_ClipByFace_t)(CMapClass *this_, const CMapFace *fa, CMapClass **f, CMapClass **b);
extern CMapSolid_ClipByFace_t CMapSolid_ClipByFace;
#endif

#ifdef USING_CMAPSOLID_CREATEFROMPLANES
typedef bool *(*CMapSolid_CreateFromPlanes_t)(CMapClass *this_, DWORD flags, void *unk);
extern CMapSolid_CreateFromPlanes_t CMapSolid_CreateFromPlanes;
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
typedef void (*CStrDlg_CStrDlg_t)(void *this_, DWORD dwFlags, const char *pszString, const char *pszPrompt, const char *pszTitle);
extern CStrDlg_CStrDlg_t CStrDlg;
#endif

#ifdef USING_CMAPCLASS_ENUMCHILDREN
typedef bool (*EnumChildrenCallback)(CMapClass *ent, void *param);
typedef bool (*CMapClass_EnumChildren_t)(void *this_, EnumChildrenCallback cb, void *param, const char *type);
extern CMapClass_EnumChildren_t CMapClass_EnumChildren;
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

#ifdef USING_CMAPDOC_SETMODIFIEDFLAG
typedef void (*CMapDoc_SetModifiedFlag_t)(void *this_, bool bModified);
extern CMapDoc_SetModifiedFlag_t CMapDoc_SetModifiedFlag;
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

#endif // HAMMERFUNCS_H
