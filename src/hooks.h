#ifndef HOOKS_H
#define HOOKS_H

#include "common.h"

enum MWMSGTYPE {
    mwStatus,
    mwError,
    mwWarning
};

extern GetMainWnd_t GetMainWnd;
extern EnumChildren_t EnumChildren;
extern SetModifiedFlag_t SetModifiedFlag;
extern CMapDoc_UpdateAllViews_t CMapDoc_UpdateAllViews;
extern CRender_DrawText_t CRender_DrawText;
extern CStrDlg_CStrDlg_t CStrDlg;
extern CDialog_DoModal_t DoModal;
extern CMapFace_SetTexture_t CMapFace_SetTexture;
extern CMapEntity_CMapEntity_t CMapEntity_CMapEntity;
extern CMapSolid_CMapSolid_t CMapSolid_CMapSolid;
extern AfxMessageBox_t AfxMessageBox;
extern Msg_t Msg;
extern ValveAlloc_t ValveAlloc;
extern CMapSolid_ClipByFace_t CMapSolid_ClipByFace;
extern CMapSolid_AddPlane_t CMapSolid_AddPlane;
extern CSolidFaces_MakeFace_t CSolidFaces_MakeFace;
extern CMapFace_CopyFrom_t CMapFace_CopyFrom;
extern CMapSolid_CreateFromPlanes_t CMapSolid_CreateFromPlanes;
extern CMapFace_CreateFace_t CMapFace_CreateFace;
extern CMapFace_InitializeTextureAxes_t CMapFace_InitializeTextureAxes;
extern CMapFace_GetOrientation_t CMapFace_GetOrientation;
extern CHistory_MarkUndoPosition_t CHistory_MarkUndoPosition;
extern CHistory_KeepNew_t CHistory_KeepNew;
extern CHistory_Keep_t CHistory_Keep;
extern GetHistory_t GetHistory;

// extern AfxGetApp_t AfxGetApp;
extern char *CMapEntityType;
extern void *selection3d;

// funcs
// extern SetPaneText_t orig_SetPaneText;
extern SetFocus_t orig_SetFocus;
extern SetActiveMapDoc_t orig_SetActiveMapDoc;
extern CFaceEditSheet_ClickFace_t orig_CFaceEditSheet_ClickFace;
extern DrawBoundsText_t orig_DrawBoundsText;
// extern CMapSolid_DoTransform_t orig_CMapSolid_DoTransform;
extern Selection3D_RenderTool2D_t orig_Selection3D_RenderTool2D;
extern EnableMenuItem_t orig_EnableMenuItem;
extern CToolMaterial_OnMouseMove3D_t orig_CToolMaterial_OnMouseMove3D;

// hooks
void hook_SetActiveMapDoc(void *doc);
void hook_DrawBoundsText(void *pRender, const float *Mins, const float *Maxs, int nFlags);
void hook_Selection3D_RenderTool2D(void *this_, void *pRender);
// void hook_CMapSolid_DoTransform(void *this_, const float *vMatrix);
void hook_CFaceEditSheet_ClickFace(void *this_, CMapClass *pSolid, int faceIndex, int cmd, int clickMode);
// int hook_SetPaneText(void *this_, int nIndex, char *lpszNewText, int bUpdate);
void *hook_SetFocus(void *this_);
bool hook_EnableMenuItem(HMENU hMenu, UINT uIDEnableItem, UINT uEnable);
bool hook_CToolMaterial_OnMouseMove3D(void *this_, void *pView, UINT flags, const float *vPoint);

CMapDoc *GetActiveMapDoc();
void *GetFaceEditSheet();
CMapClass *new_CMapEntity();
CMapClass *new_CMapSolid();
int AfxMessageBoxF(UINT nType, const char* fmt, ...);

#endif // HOOKS_H
