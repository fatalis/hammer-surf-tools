#include "hooks.h"
#include "measure.h"
#include "contextmenu.h"
#include "hotkeys.h"
#include "rampgen.h"

#ifdef USING_HOOK_AFXWNDPROC
AfxWndProc_t orig_AfxWndProc;

LRESULT hook_AfxWndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) {
    LRESULT result = orig_AfxWndProc(hWnd, Msg, wParam, lParam);

    if (Msg == WM_COMMAND) {
        UINT cmd = LOWORD(wParam);
        menu_check_command(cmd);
    }

    return result;
}
#endif

#ifdef USING_HOOK_CMAPDOC_SETACTIVEMAPDOC
static CMapDoc *active_map_doc;
SetActiveMapDoc_t orig_SetActiveMapDoc;
// hook because GetActiveMapDoc is compiled away
void hook_SetActiveMapDoc(void *doc) {
    /* log_msg("[hook] SetActiveMapDoc %p\n", doc); */
    orig_SetActiveMapDoc(doc);
    active_map_doc = doc;
}

CMapDoc *GetActiveMapDoc() {
    return active_map_doc;
}
#endif


#ifdef USING_HOOK_CFACEEDITSHEET_CLICKFACE
CFaceEditSheet_ClickFace_t orig_CFaceEditSheet_ClickFace;

void hook_CFaceEditSheet_ClickFace(void *this_, CMapClass *pSolid, int faceIndex, int cmd, int clickMode) {
    orig_CFaceEditSheet_ClickFace(this_, pSolid, faceIndex, cmd, clickMode);
}
#endif

// fix an annoyance
#ifdef USING_HOOK_CWND_SETFOCUS
SetFocus_t orig_SetFocus;

void *hook_SetFocus(void *this_) {
    /* log_msg("[hook SetFocus %p\n", this_); */

    void *mainWnd = GetMainWnd();

    if (mainWnd) {
        void *ObjectProperties = *(void **)(mainWnd + CMAINFRAME_OFFSET_OBJECTPROPERTIES);

        if (ObjectProperties) {
            if (this_ != ObjectProperties) {
                HWND properties_hwnd = *(HWND *)(ObjectProperties + CWND_OFFSET_HWND);
                bool visible = IsWindowVisible(properties_hwnd);

                if (visible) {
                    // log_msg("[hook] blocking SetFocus while ObjectProperties visible %p %p\n", this_, ObjectProperties);
                    return nullptr;
                }
            }
        }
    }

    return orig_SetFocus(this_);
}
#endif

#ifdef USING_HOOK_ENABLEMENUITEM
EnableMenuItem_t orig_EnableMenuItem;

bool hook_EnableMenuItem(HMENU hMenu, UINT uIDEnableItem, UINT uEnable) {
    uEnable = decide_menu_item_enabled(hMenu, uIDEnableItem, uEnable);
    return orig_EnableMenuItem(hMenu, uIDEnableItem, uEnable);
}
#endif

#ifdef USING_HOOK_LOADMENUW
LoadMenuW_t orig_LoadMenuW;

HMENU hook_LoadMenuW(HINSTANCE hInstance, LPCWSTR lpMenuName) {
    HMENU menu = orig_LoadMenuW(hInstance, lpMenuName);
    check_add_menus(menu, lpMenuName);
    return menu;
}
#endif

#ifdef USING_HOOK_LOADACCELERATORSA
LoadAcceleratorsA_t orig_LoadAcceleratorsA;

HACCEL hook_LoadAcceleratorsA(HINSTANCE hInstance, LPCSTR lpTableName) {
    HACCEL accel = orig_LoadAcceleratorsA(hInstance, lpTableName);
    accel = check_add_hotkeys(accel, lpTableName);
    return accel;
}
#endif

#ifdef USING_HOOK_SELECTION3D_RENDERTOOL2D
Selection3D_RenderTool2D_t orig_Selection3D_RenderTool2D;

void hook_Selection3D_RenderTool2D(void *this_, void *pRender) {
    orig_Selection3D_RenderTool2D(this_, pRender);
    measure_render_2d(this_, pRender);
}
#endif

#ifdef USING_HOOK_CHISTORY_MARKUNDOPOSITION
CHistory_MarkUndoPosition_t CHistory_MarkUndoPosition;

void hook_CHistory_MarkUndoPosition(CHistory *this_, const void* pSelection, const char *pszName, bool bFromOpposite) {
    rampgen_close();
    CHistory_MarkUndoPosition(this_, pSelection, pszName, bFromOpposite);
}
#endif

#ifdef USING_HOOK_CLIPPER3D_DRAWBRUSHEXTENTS
Clipper3D_DrawBrushExtents_t orig_Clipper3D_DrawBrushExtents;

void hook_Clipper3D_DrawBrushExtents(void *this_, void *pRender, void *pSolid, int nFlags) {
    orig_Clipper3D_DrawBrushExtents(this_, pRender, pSolid, nFlags);
    measure_clipper_plane(this_, pRender);
}
#endif
