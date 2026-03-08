#include "hooks.h"
#include "patterns.h"
#include "triggergen.h"
#include "hammerfuncs.h"
#include "measure.h"

static CMapDoc *active_map_doc;

// TODO: dont do this
static void *face_edit_sheet;
// TODO: dont do this
static void *selection3d;


Clipper3D_DrawBrushExtents_t orig_Clipper3D_DrawBrushExtents;
CFaceEditSheet_ClickFace_t orig_CFaceEditSheet_ClickFace;
// CToolMaterial_OnMouseMove3D_t orig_CToolMaterial_OnMouseMove3D;
EnableMenuItem_t orig_EnableMenuItem;
Selection3D_RenderTool2D_t orig_Selection3D_RenderTool2D;
SetActiveMapDoc_t orig_SetActiveMapDoc;
// SetFocus_t orig_SetFocus;

// hook because the getter func is too short to locate
void hook_SetActiveMapDoc(void *doc) {
    /* log_msg("[hook] SetActiveMapDoc %p\n", doc); */
    orig_SetActiveMapDoc(doc);
    active_map_doc = doc;
}

CMapDoc *GetActiveMapDoc() {
    return active_map_doc;
}

// TODO: dont do this
void *GetFaceEditSheet() {
    return face_edit_sheet;
}

// TODO: dont do this
void *GetSelection3D() {
    return selection3d;
}

// enum {
//     cfToggle    = 0x01,			// toggle - if selected, then unselect
//     cfSelect    = 0x02,			// select
//     cfUnselect  = 0x04,			// unselect
//     cfClear     = 0x08,			// clear face list
//     cfEdgeAlign = 0x10			// align face texture coordinates to 3d view alignment - should be here???
// };

void hook_CFaceEditSheet_ClickFace(void *this_, CMapClass *pSolid, int faceIndex, int cmd, int clickMode) {
    orig_CFaceEditSheet_ClickFace(this_, pSolid, faceIndex, cmd, clickMode);
    face_edit_sheet = this_;
}

// fix an annoyance
// void *hook_SetFocus(void *this_) {
//     /* log_msg("[hook SetFocus %p\n", this_); */
//
//     void *mainWnd = GetMainWnd();
//
//     if (mainWnd) {
//         void *ObjectProperties = *(void **)(mainWnd + OFFSET_OBJECT_PROPERTIES);
//
//         if (ObjectProperties) {
//             if (this_ != ObjectProperties) {
//                 HWND properties_hwnd = *(HWND *)(ObjectProperties + OFFSET_HWND);
//                 bool visible = IsWindowVisible(properties_hwnd);
//
//                 if (visible) {
//                     // log_msg("[hook] blocking SetFocus while ObjectProperties visible %p %p\n", this_, ObjectProperties);
//                     return nullptr;
//                 }
//             }
//         }
//     }
//
//     return orig_SetFocus(this_);
// }

bool hook_EnableMenuItem(HMENU hMenu, UINT uIDEnableItem, UINT uEnable) {
    // log_msg("EnableMenuItem %p %p %p %p\n", hMenu, uIDEnableItem, uEnable, 42069);
    if (uEnable & MF_BYPOSITION) {
        UINT id = GetMenuItemID(hMenu, (int)uIDEnableItem);
        if (id == CMD_CURVED_RAMP_GENERATOR || id == CMD_ANGLEFIX || id == CMD_TRIGGER_GENERATOR) {
            uEnable = MF_ENABLED;
        }
    }

    return orig_EnableMenuItem(hMenu, uIDEnableItem, uEnable);
}

// bool hook_CToolMaterial_OnMouseMove3D(void *this_, void *pView, UINT flags, const float *vPoint) {
//     // log_msg("CToolMaterial_OnMouseMove3D %p %p %d %f %f\n", this_, pView, flags, (double)vPoint[0], (double)vPoint[1]);
//     return orig_CToolMaterial_OnMouseMove3D(this_, pView, flags, vPoint);
// }

void hook_Selection3D_RenderTool2D(void *this_, void *pRender) {
    /* log_msg("[hook] Selection3D::RenderTool2D %p %p\n", this_, pRender); */
    orig_Selection3D_RenderTool2D(this_, pRender);
    selection3d = this_;

    measure_render_2d(this_, pRender);
}

void hook_Clipper3D_DrawBrushExtents(void *this_, void *pRender, void *pSolid, int nFlags) {
    measure_clipper_plane(this_, pRender);
    orig_Clipper3D_DrawBrushExtents(this_, pRender, pSolid, nFlags);
}
