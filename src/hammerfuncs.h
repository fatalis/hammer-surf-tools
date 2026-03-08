#ifndef HAMMERFUNCS_H
#define HAMMERFUNCS_H

#include "common.h"

typedef int (*AfxMessageBox_t)(LPCTSTR lpszText, UINT nType, UINT nIDHelp);
extern AfxMessageBox_t AfxMessageBox;

typedef int (*CDialog_DoModal_t)(void *this_);
extern CDialog_DoModal_t DoModal;

typedef void (*CHistory_KeepNew_t)(void *this_, CMapClass *pObject, bool bKeepChildren);
extern CHistory_KeepNew_t CHistory_KeepNew;

typedef void (*CHistory_Keep_t)(void *this_, CMapClass *pObject);
extern CHistory_Keep_t CHistory_Keep;

typedef void (*CHistory_MarkUndoPosition_t)(void *this_, const void* pSelection, const char *pszName, bool);
extern CHistory_MarkUndoPosition_t CHistory_MarkUndoPosition;

#define MAPVIEW_UPDATE_OBJECTS 0x001
#define MAPVIEW_RENDER_NOW     0x800
typedef void (*CMapDoc_UpdateAllViews_t)(void *this_, int nFlags, void *ub);
extern CMapDoc_UpdateAllViews_t CMapDoc_UpdateAllViews;

typedef void (*CMapEntity_CMapEntity_t)(void *this_);
extern CMapEntity_CMapEntity_t CMapEntity_CMapEntity;

// typedef CMapFace *(*CMapFace_CopyFrom_t)(CMapFace *this_, const CMapFace *from, DWORD dwFlags, bool bUpdateDependencies);
// extern CMapFace_CopyFrom_t CMapFace_CopyFrom;

typedef bool (*CMapFace_CreateFace_t)(CMapFace *this_, Vec3 *points, int npoints, bool bIsCordonFace);
extern CMapFace_CreateFace_t CMapFace_CreateFace;

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

typedef void (*CMapFace_SetTexture_t)(void *this_, const char *pszNewTex, bool bRescaleTextureCoordinates);
extern CMapFace_SetTexture_t CMapFace_SetTexture;

// typedef bool (*CMapSolid_AddPlane_t)(CMapClass *this_, const CMapFace *fa);
// extern CMapSolid_AddPlane_t CMapSolid_AddPlane;

typedef void (*CMapSolid_CMapSolid_t)(void *this_, CMapClass *parent);
extern CMapSolid_CMapSolid_t CMapSolid_CMapSolid;

// typedef void (*CMapSolid_ClipByFace_t)(CMapClass *this_, const CMapFace *fa, CMapClass **f, CMapClass **b);
// extern CMapSolid_ClipByFace_t CMapSolid_ClipByFace;

// typedef bool *(*CMapSolid_CreateFromPlanes_t)(CMapClass *this_, DWORD flags, void *unk);
// extern CMapSolid_CreateFromPlanes_t CMapSolid_CreateFromPlanes;

typedef void (*CRender_DrawText_t)(void *this_, const char *text, int x, int y, int nFlags);
extern CRender_DrawText_t CRender_DrawText;

typedef CMapFace *(*CSolidFaces_MakeFace_t)(void *this_);
extern CSolidFaces_MakeFace_t CSolidFaces_MakeFace;

typedef void (*CStrDlg_CStrDlg_t)(void *this_, DWORD dwFlags, const char *pszString, const char *pszPrompt, const char *pszTitle);
extern CStrDlg_CStrDlg_t CStrDlg;

typedef bool (*EnumChildrenCallback)(CMapClass *ent, void *param);
typedef bool (*EnumChildren_t)(void *this_, EnumChildrenCallback cb, void *param, const char *type);
extern EnumChildren_t EnumChildren;

typedef void *(*GetHistory_t)();
extern GetHistory_t GetHistory;

// typedef void *(*GetMainWnd_t)();
// extern GetMainWnd_t GetMainWnd;

enum MWMSGTYPE {
    mwStatus,
    mwError,
    mwWarning
};
typedef void (*Msg_t)(int type, const char *fmt, ...);
extern Msg_t Msg;

typedef void (*SetModifiedFlag_t)(void *this_, bool bModified);
extern SetModifiedFlag_t SetModifiedFlag;

typedef void *(*ValveAlloc_t)(size_t size);
extern ValveAlloc_t ValveAlloc;

#endif // HAMMERFUNCS_H
