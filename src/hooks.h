#ifndef HOOKS_H
#define HOOKS_H

#include "common.h"

typedef void (*Clipper3D_DrawBrushExtents_t)(void *this_, void *pRender, void *pSolid, int nFlags);
extern Clipper3D_DrawBrushExtents_t orig_Clipper3D_DrawBrushExtents;
void hook_Clipper3D_DrawBrushExtents(void *this_, void *pRender, void *pSolid, int nFlags);

// typedef void *(*SetFocus_t)(void *this_);
// extern SetFocus_t orig_SetFocus;
// void *hook_SetFocus(void *this_);

typedef void (*SetActiveMapDoc_t)(void *doc);
extern SetActiveMapDoc_t orig_SetActiveMapDoc;
void hook_SetActiveMapDoc(void *doc);

typedef void (*CFaceEditSheet_ClickFace_t)(void *this_, CMapClass *pSolid, int faceIndex, int cmd, int clickMode);
extern CFaceEditSheet_ClickFace_t orig_CFaceEditSheet_ClickFace;
void hook_CFaceEditSheet_ClickFace(void *this_, CMapClass *pSolid, int faceIndex, int cmd, int clickMode);

typedef bool (*EnableMenuItem_t)(HMENU hMenu, UINT uIDEnableItem, UINT uEnable);
extern EnableMenuItem_t orig_EnableMenuItem;
bool hook_EnableMenuItem(HMENU hMenu, UINT uIDEnableItem, UINT uEnable);

// typedef bool (*CToolMaterial_OnMouseMove3D_t)(void *this_, void *pView, UINT flags, const float *vPoint);
// extern CToolMaterial_OnMouseMove3D_t orig_CToolMaterial_OnMouseMove3D;
// bool hook_CToolMaterial_OnMouseMove3D(void *this_, void *pView, UINT flags, const float *vPoint);

typedef void (*Selection3D_RenderTool2D_t)(void *this_, void *pRender);
extern Selection3D_RenderTool2D_t orig_Selection3D_RenderTool2D;
void hook_Selection3D_RenderTool2D(void *this_, void *pRender);


CMapDoc *GetActiveMapDoc();
void *GetFaceEditSheet();
void *GetSelection3D();

#endif // HOOKS_H
