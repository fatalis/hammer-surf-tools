#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-conversion"
#pragma GCC diagnostic ignored "-Wconversion"
#define STB_DS_IMPLEMENTATION
#include <stb_ds.h>
#pragma GCC diagnostic pop

#define DVECTOR_IMPLEMENTATION
#include <dvector.h>

#include "util.h"

// TODO: put in a struct?
MAPCLASSTYPE CMapWorldType;
MAPCLASSTYPE CMapEntityType;
MAPCLASSTYPE CMapSolidType;

#ifndef TEST_PATTERNS
static void get_exe_directory(char* out, size_t size) {
    DWORD len = GetModuleFileNameA(NULL, out, (DWORD)size);
    if (len == 0 || len == size) {
        // error or truncated
        out[0] = '\0';
        return;
    }

    // strip filename
    for (int i = (int)len - 1; i >= 0; --i) {
        if (out[i] == '\\' || out[i] == '/') {
            out[i] = '\0';
            break;
        }
    }
}
#endif

void log_msg(const char *fmt, ...) {
#ifdef TEST_PATTERNS
    va_list va;
    va_start(va, fmt);
    vprintf(fmt, va);
    va_end(va);
#else
    // cwd changes so have to do this
    char dir[MAX_PATH];
    get_exe_directory(dir, sizeof(dir));
    char path[MAX_PATH];
    snprintf(path, sizeof(path), "%s\\%s", dir, "surf-tools.log");

    FILE *f = fopen(path, "a");
    if (!f)
        return;

    va_list va;
    va_start(va, fmt);
    vfprintf(f, fmt, va);
    va_end(va);

    fclose(f);
#endif
}

bool surf_tools_init() {
    CMapWorldType  = MAPCLASS_TYPE("CMapWorld");
    CMapEntityType = MAPCLASS_TYPE("CMapEntity");
    CMapSolidType  = MAPCLASS_TYPE("CMapSolid");
    return CMapWorldType && CMapEntityType && CMapSolidType;
}
