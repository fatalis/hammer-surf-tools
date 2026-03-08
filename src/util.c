#include "util.h"
#include "hammerfuncs.h"

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

RefVector *CMapDoc_GetSelection(CMapDoc *doc) {
    return &doc->m_pSelection->m_SelectionList;
}
