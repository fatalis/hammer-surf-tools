#include "hooks.h"
#include "patterns.h"
#include "triggergen.h"

// TODO: move everything in here to their own files
// TODO: this is basically misc.c rn

GetMainWnd_t GetMainWnd;
EnumChildren_t EnumChildren;
SetModifiedFlag_t SetModifiedFlag;
CMapDoc_UpdateAllViews_t CMapDoc_UpdateAllViews;
CStrDlg_CStrDlg_t CStrDlg;
CDialog_DoModal_t DoModal;
CMapFace_SetTexture_t CMapFace_SetTexture;
CMapEntity_CMapEntity_t CMapEntity_CMapEntity;
CMapSolid_CMapSolid_t CMapSolid_CMapSolid;
AfxMessageBox_t AfxMessageBox;
Msg_t Msg;
ValveAlloc_t ValveAlloc;
CMapSolid_ClipByFace_t CMapSolid_ClipByFace;
CMapSolid_AddPlane_t CMapSolid_AddPlane;
CSolidFaces_MakeFace_t CSolidFaces_MakeFace;
CMapFace_CopyFrom_t CMapFace_CopyFrom;
CMapSolid_CreateFromPlanes_t CMapSolid_CreateFromPlanes;
CMapFace_CreateFace_t CMapFace_CreateFace;
CMapFace_InitializeTextureAxes_t CMapFace_InitializeTextureAxes;
CMapFace_GetOrientation_t CMapFace_GetOrientation;
CHistory_MarkUndoPosition_t CHistory_MarkUndoPosition;
CHistory_KeepNew_t CHistory_KeepNew;
CHistory_Keep_t CHistory_Keep;
GetHistory_t GetHistory;

// AfxGetApp_t AfxGetApp;
char *CMapEntityType;
static CMapDoc *active_map_doc;
// TODO: dont do this
static void *face_edit_sheet;

// SetPaneText_t orig_SetPaneText;
SetFocus_t orig_SetFocus;
SetActiveMapDoc_t orig_SetActiveMapDoc;
CFaceEditSheet_ClickFace_t orig_CFaceEditSheet_ClickFace;
// CMapSolid_DoTransform_t orig_CMapSolid_DoTransform;
EnableMenuItem_t orig_EnableMenuItem;
CToolMaterial_OnMouseMove3D_t orig_CToolMaterial_OnMouseMove3D;

// static void *status_bar;

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

// void hook_CMapSolid_DoTransform(void *this_, const float *vMatrix) {
//     /* log_msg("[hook] CMapSolid::DoTransform\n"); */
//     orig_CMapSolid_DoTransform(this_, vMatrix);
// }

enum {
    cfToggle    = 0x01,			// toggle - if selected, then unselect
    cfSelect    = 0x02,			// select
    cfUnselect  = 0x04,			// unselect
    cfClear     = 0x08,			// clear face list
    cfEdgeAlign = 0x10			// align face texture coordinates to 3d view alignment - should be here???
};

void hook_CFaceEditSheet_ClickFace(void *this_, CMapClass *pSolid, int faceIndex, int cmd, int clickMode) {
    orig_CFaceEditSheet_ClickFace(this_, pSolid, faceIndex, cmd, clickMode);

    face_edit_sheet = this_;

    // log_msg("cmd %d %d %d\n", faceIndex, cmd, clickMode);
    if (cmd == (cfToggle | cfClear )) {
        CMapFace *face = &pSolid->Faces.list[faceIndex];
        // CreateTriggerExtrudedFromFace(face);

        Vec3 *normal = &face->plane.normal;
        Vec3 *p = face->plane.points;
        log_msg(
            "%p clicked face %d %p norm %g %g %g pts [%g %g %g] [%g %g %g] [%g %g %g]\n",
            this_,
            faceIndex,
            face,
            (double)normal->x, (double)normal->y, (double)normal->z,
            (double)p[0].x, (double)p[0].y, (double)p[0].z,
            (double)p[1].x, (double)p[1].y, (double)p[1].z,
            (double)p[2].x, (double)p[2].y, (double)p[2].z
        );
    }
}

//int hook_SetPaneText(void *this_, int nIndex, char *lpszNewText, int bUpdate) {
//    /* log_msg("[SetPaneText] %p %d \"%s\" %d\n", this_, nIndex, lpszNewText, bUpdate); */
//    status_bar = this_;
//
//    return orig_SetPaneText(this_, nIndex, lpszNewText, bUpdate);
//}

void *hook_SetFocus(void *this_) {
    /* log_msg("[hook SetFocus %p\n", this_); */

    void *mainWnd = GetMainWnd();

    if (mainWnd) {
        void *ObjectProperties = *(void **)(mainWnd + OFFSET_OBJECT_PROPERTIES);

        if (ObjectProperties) {
            if (this_ != ObjectProperties) {
                HWND properties_hwnd = *(HWND *)(ObjectProperties + OFFSET_HWND);
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

bool hook_CToolMaterial_OnMouseMove3D(void *this_, void *pView, UINT flags, const float *vPoint) {
    // log_msg("CToolMaterial_OnMouseMove3D %p %p %d %f %f\n", this_, pView, flags, (double)vPoint[0], (double)vPoint[1]);
    return orig_CToolMaterial_OnMouseMove3D(this_, pView, flags, vPoint);
}

CMapClass *new_CMapEntity() {
    CMapClass *ent = ValveAlloc(CMAPENTITY_SIZE);
    CMapEntity_CMapEntity(ent);
    return ent;
}

CMapClass *new_CMapSolid() {
    CMapClass *ent = ValveAlloc(CMAPSOLID_SIZE);
    CMapSolid_CMapSolid(ent, nullptr);
    return ent;
}

int AfxMessageBoxF(UINT nType, const char* fmt, ...) {
    const int BUFFER_SIZE = 512;
    char buffer[BUFFER_SIZE];

    va_list args;
    va_start(args, fmt);
    vsnprintf(buffer, BUFFER_SIZE, fmt, args);
    va_end(args);

    return AfxMessageBox(buffer, nType, 0);
}
