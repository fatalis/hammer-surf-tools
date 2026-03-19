#include "util.h"
#include "hammerfuncs.h"
#include "hooks.h"

static char str_buffer[1024];

static HINSTANCE hInstance;
void SetHInstance(HINSTANCE hInst) {
    hInstance = hInst;
}
HINSTANCE GetHInstance() {
    return hInstance;
}

MAPCLASSTYPE MAPCLASS_TYPE(const char* type_name) {
    for (int i = 0; i < S_Classes->length; i++) {
        MCMSTRUCT e = S_Classes->items[i];
        if (!strcmp(e.Type, type_name)) {
            return e.Type;
        }
    }
    
    return NULL;
}

int AfxMessageBoxF(UINT nType, const char* fmt, ...) {
    const size_t BUFFER_SIZE = 512;
    char buffer[BUFFER_SIZE];

    va_list args;
    va_start(args, fmt);
    vsnprintf(buffer, BUFFER_SIZE, fmt, args);
    va_end(args);

    return AfxMessageBox(buffer, nType, 0);
}

CMapEntity *new_CMapEntity() {
    CMapEntity *ent = ValveAlloc(CMAPENTITY_SIZE);
    CMapEntityMethods.CMapEntity(ent);
    return ent;
}

CMapSolid *new_CMapSolid() {
    CMapSolid *ent = ValveAlloc(CMAPSOLID_SIZE);
    CMapSolidMethods.CMapSolid(ent, nullptr);
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

CMapSolid *CMapClass_AsSolid(CMapClass *ent) {
    if (!ent) {
        return nullptr;
    }
    const char *name = ent->vtable->GetType(ent);
    return !strcmp(name, "CMapSolid") ? (CMapSolid *)ent : nullptr;
}

bool CMapClass_IsWorldBrush(CMapClass *ent) {
    if (CMapClass_AsSolid(ent)) {
        CMapClass *parent = ent->vtable->GetParent(ent);
        if (parent && !strcmp(parent->vtable->GetType(parent), "CMapWorld")) {
            return true;
        }
    }
    return false;
}

void CMapEntity_SetKVOrigin(CMapEntity *ent) {
    CEditGameClass *edit = &ent->m_EditGameClass;
    char origin[KEYVALUE_MAX_VALUE_LENGTH];
    snprintf(origin, sizeof(origin), "%g %g %g", (double)ent->base.point.m_Origin.x, (double)ent->base.point.m_Origin.y, (double)ent->base.point.m_Origin.z);
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

static int debug_point_id;
void debug_point(int id, Vec3 *point, uint32_t color) {
    debug_point_id++;
    CMapDoc *doc = GetActiveMapDoc();

    CMapEntity *ent = new_CMapEntity();
    CEditGameClass *edit = &ent->m_EditGameClass;

    *(WORD *)((void *)ent + CMAPENTITY_OFFSET_FLAGS) = 1; // placeholder flag
    // crashes if not main thread
    edit->vtable->SetClass(edit, "info_target", false);

    char name[128];
    snprintf(name, sizeof(name), "[%d %d] %g %g %g\n", debug_point_id, id, (double)point->x, (double)point->y, (double)point->z);

    edit->vtable->SetKeyValue(edit, "targetname", name);
    ent->base.vtable->SetOrigin(ent, point);
    doc->vtable->AddObjectToWorld(doc, ent, nullptr);

    ent->base.vtable->SetRenderColor(ent, color);

    CHistory_KeepNew(GetHistory(), (CMapClass *)ent, false);
}

char *dump_plane(Plane *plane) {
    snprintf(str_buffer, sizeof(str_buffer), "Plane: normal=(%d %d %d) dist=%d pts=[%d %d %d, %d %d %d, %d %d %d]",
            (int)plane->normal.x,
            (int)plane->normal.y,
            (int)plane->normal.z,
            (int)plane->dist,
            (int)plane->points[0].x,
            (int)plane->points[0].y,
            (int)plane->points[0].z,
            (int)plane->points[1].x,
            (int)plane->points[1].y,
            (int)plane->points[1].z,
            (int)plane->points[2].x,
            (int)plane->points[2].y,
            (int)plane->points[2].z);
    return str_buffer;
}
