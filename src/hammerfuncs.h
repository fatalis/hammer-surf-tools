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

typedef void (*CMapDoc_UpdateAllViews_t)(void *this_, int nFlags, void *ub);
extern CMapDoc_UpdateAllViews_t CMapDoc_UpdateAllViews;

typedef void (*CMapEntity_CMapEntity_t)(void *this_);
extern CMapEntity_CMapEntity_t CMapEntity_CMapEntity;

// typedef CMapFace *(*CMapFace_CopyFrom_t)(CMapFace *this_, const CMapFace *from, DWORD dwFlags, bool bUpdateDependencies);
// extern CMapFace_CopyFrom_t CMapFace_CopyFrom;

typedef bool (*CMapFace_CreateFace_t)(CMapFace *this_, Vec3 *points, int npoints, bool bIsCordonFace);
extern CMapFace_CreateFace_t CMapFace_CreateFace;

typedef FaceOrientation (*CMapFace_GetOrientation_t)(void *this_);
extern CMapFace_GetOrientation_t CMapFace_GetOrientation;

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


int AfxMessageBoxF(UINT nType, const char* fmt, ...);
CMapClass *new_CMapEntity();
CMapClass *new_CMapSolid();

#endif // HAMMERFUNCS_H
