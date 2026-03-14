#include "util.h"
#include "hammerfuncs.h"

int AfxMessageBoxF(UINT nType, const char* fmt, ...) {
    const size_t BUFFER_SIZE = 512;
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

CMapObjectList *CMapDoc_GetSelection(CMapDoc *doc) {
    return &doc->m_pSelection->m_SelectionList;
}

void *GetFaceEditSheet() {
    void *wnd = GetMainWnd();
    if (!wnd) {
        return nullptr;
    }
    return *(void **)((void *)wnd + CMainFrame_m_pFaceEditSheet_Offset);
}

HWND GetMainWndHwnd() {
    void *wnd = GetMainWnd();
    if (!wnd) {
        return nullptr;
    }
    return *(void **)((void *)wnd + CWND_OFFSET_HWND);
}

StoredFaceVector *CFaceEditSheet_GetFaces(void *sheet) {
    return (void *)sheet + CFaceEditSheet_m_Faces_Offset;
}

bool CMapClass_IsSolid(CMapClass *ent) {
    char *name = ent->vtable->GetType(ent);
    return !strcmp(name, "CMapSolid");
}

bool CMapClass_IsWorldBrush(CMapClass *ent) {
    if (CMapClass_IsSolid(ent)) {
        CMapClass *parent = ent->vtable->GetParent(ent);
        if (parent && !strcmp(parent->vtable->GetType(parent), "CMapWorld")) {
            return true;
        }
    }
    return false;
}

void CMapEntity_SetKVOrigin(CMapClass *ent) {
    CEditGameClass *edit = &ent->m_EditGameClass;
    char origin[KEYVALUE_MAX_VALUE_LENGTH];
    snprintf(origin, sizeof(origin), "%g %g %g", (double)ent->m_Origin.x, (double)ent->m_Origin.y, (double)ent->m_Origin.z);
    edit->vtable->SetKeyValue(edit, "origin", origin);
}

bool IsAllWorldBrushes(CMapObjectList *selected) {
    int count = 0;
    for (auto i = 0; i < selected->length; i++) {
        if (CMapClass_IsWorldBrush(selected->items[i])) {
            count++;
        }
    };
    return count == selected->length;
}
