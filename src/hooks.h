#ifndef HOOKS_H
#define HOOKS_H

#ifdef USING_HOOK_AFXWNDPROC
typedef LRESULT (*AfxWndProc_t)(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
extern AfxWndProc_t orig_AfxWndProc;
LRESULT hook_AfxWndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
#endif

#ifdef USING_HOOK_CLIPPER3D_DRAWBRUSHEXTENTS
typedef void (*Clipper3D_DrawBrushExtents_t)(void *this_, void *pRender, void *pSolid, int nFlags);
extern Clipper3D_DrawBrushExtents_t orig_Clipper3D_DrawBrushExtents;
void hook_Clipper3D_DrawBrushExtents(void *this_, void *pRender, void *pSolid, int nFlags);
#endif

#ifdef USING_HOOK_CWND_SETFOCUS
typedef void *(*SetFocus_t)(void *this_);
extern SetFocus_t orig_SetFocus;
void *hook_SetFocus(void *this_);
#endif

#ifdef USING_HOOK_CMAPDOC_SETACTIVEMAPDOC
typedef void (*SetActiveMapDoc_t)(void *doc);
extern SetActiveMapDoc_t orig_SetActiveMapDoc;
void hook_SetActiveMapDoc(void *doc);
CMapDoc *GetActiveMapDoc();
#endif

#ifdef USING_HOOK_CFACEEDITSHEET_CLICKFACE
enum {
    cfToggle    = 0x01,			// toggle - if selected, then unselect
    cfSelect    = 0x02,			// select
    cfUnselect  = 0x04,			// unselect
    cfClear     = 0x08,			// clear face list
    cfEdgeAlign = 0x10			// align face texture coordinates to 3d view alignment - should be here???
};

typedef void (*CFaceEditSheet_ClickFace_t)(void *this_, CMapClass *pSolid, int faceIndex, int cmd, int clickMode);
extern CFaceEditSheet_ClickFace_t orig_CFaceEditSheet_ClickFace;
void hook_CFaceEditSheet_ClickFace(void *this_, CMapClass *pSolid, int faceIndex, int cmd, int clickMode);
#endif

#ifdef USING_HOOK_ENABLEMENUITEM
typedef bool (*EnableMenuItem_t)(HMENU hMenu, UINT uIDEnableItem, UINT uEnable);
extern EnableMenuItem_t orig_EnableMenuItem;
bool hook_EnableMenuItem(HMENU hMenu, UINT uIDEnableItem, UINT uEnable);
#endif

#ifdef USING_HOOK_LOADMENUW
typedef HMENU (*LoadMenuW_t)(HINSTANCE hInstance, LPCWSTR lpMenuName);
extern LoadMenuW_t orig_LoadMenuW;
HMENU hook_LoadMenuW(HINSTANCE hInstance, LPCWSTR lpMenuName);
#endif

#ifdef USING_HOOK_LOADACCELERATORSA
typedef HACCEL (*LoadAcceleratorsA_t)(HINSTANCE hInstance, LPCSTR lpTableName);
extern LoadAcceleratorsA_t orig_LoadAcceleratorsA;
HACCEL hook_LoadAcceleratorsA(HINSTANCE hInstance, LPCSTR lpTableName);
#endif

#ifdef USING_HOOK_SELECTION3D_RENDERTOOL2D
typedef void (*Selection3D_RenderTool2D_t)(void *this_, void *pRender);
extern Selection3D_RenderTool2D_t orig_Selection3D_RenderTool2D;
void hook_Selection3D_RenderTool2D(void *this_, void *pRender);
#endif

#endif // HOOKS_H
