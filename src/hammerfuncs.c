#include "hammerfuncs.h"

AfxMessageBox_t AfxMessageBox;
CDialog_DoModal_t DoModal;
CHistory_KeepNew_t CHistory_KeepNew;
CHistory_Keep_t CHistory_Keep;
CHistory_MarkUndoPosition_t CHistory_MarkUndoPosition;
CMapDoc_UpdateAllViews_t CMapDoc_UpdateAllViews;
CMapEntity_CMapEntity_t CMapEntity_CMapEntity;
// CMapFace_CopyFrom_t CMapFace_CopyFrom;
CMapFace_CreateFace_t CMapFace_CreateFace;
CMapFace_GetOrientation_t CMapFace_GetOrientation;
CMapFace_InitializeTextureAxes_t CMapFace_InitializeTextureAxes;
CMapFace_SetTexture_t CMapFace_SetTexture;
// CMapSolid_AddPlane_t CMapSolid_AddPlane;
CMapSolid_CMapSolid_t CMapSolid_CMapSolid;
// CMapSolid_ClipByFace_t CMapSolid_ClipByFace;
// CMapSolid_CreateFromPlanes_t CMapSolid_CreateFromPlanes;
CRender_DrawText_t CRender_DrawText;
CSolidFaces_MakeFace_t CSolidFaces_MakeFace;
CStrDlg_CStrDlg_t CStrDlg;
EnumChildren_t EnumChildren;
GetHistory_t GetHistory;
// GetMainWnd_t GetMainWnd;
Msg_t Msg;
SetModifiedFlag_t SetModifiedFlag;
ValveAlloc_t ValveAlloc;

int AfxMessageBoxF(UINT nType, const char* fmt, ...) {
    const int BUFFER_SIZE = 512;
    char buffer[BUFFER_SIZE];

    va_list args;
    va_start(args, fmt);
    vsnprintf(buffer, BUFFER_SIZE, fmt, args);
    va_end(args);

    return AfxMessageBox(buffer, nType, 0);
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
