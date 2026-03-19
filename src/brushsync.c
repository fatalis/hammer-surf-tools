#include "brushsync.h"
#include "hooks.h"
#include "scriptfuncs.h"
#include "hammerfuncs.h"

// moves brushes in hammer when moved ingame with a vscript
// proof of concept, vscript code isn't finished

typedef struct {
    int x;
    int y;
    int z;
} Int3;

typedef struct {
    Int3 pos;
    CMapClass *ent;
} FindEntity;

static bool find_ent_by_pos(CMapClass *ent, void *param) {
    FindEntity *find = param;

    const char *name = ent->vtable->GetType(ent);
    if (!strcmp(name, "CMapEntity")) {
        Int3 rounded = {
            (int)roundf(ent->point.m_Origin.x),
            (int)roundf(ent->point.m_Origin.y),
            (int)roundf(ent->point.m_Origin.z)
        };

        if (rounded.x == find->pos.x && rounded.y == find->pos.y && rounded.z == find->pos.z) {
            find->ent = ent;
            return false;
        }
    }

    return true;
}

static void move_brush(Int3 start, Int3 end) {
    CMapDoc *doc = GetActiveMapDoc();
    if (!doc || !doc->m_pWorld) {
        return;
    }

    FindEntity find;
    find.pos = start;
    find.ent = nullptr;

    CMapClassMethods.EnumChildren(doc->m_pWorld, find_ent_by_pos, &find, nullptr);
    /* log_msg("[hook] FindEnt = %p\n", find.ent); */

    if (find.ent) {
        Vec3 delta;
        TransMove(find.ent, &delta);
        log_msg("[hook] moved %p with delta %.1f %.1f %.1f\n", find.ent, (double)delta.x, (double)delta.y, (double)delta.z);
        CMapDocMethods.SetModifiedFlag(doc, true);
        CMapDocMethods.UpdateAllViews(doc, MAPVIEW_UPDATE_OBJECTS, nullptr);
    } else {
        log_msg("[hook] failed to find ent at %d %d %d\n", find.pos.x, find.pos.y, find.pos.z);
    }
}

int follow_console_log() {
    // fopen/crt version doesnt work, raw win32 apis work though?
    const char * path = "..\\..\\cstrike\\console.log";
    HANDLE hFile = CreateFileA(
        path,
        GENERIC_READ,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        nullptr,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        nullptr
    );

    if (hFile == INVALID_HANDLE_VALUE) {
        log_msg("error opening console.log\n");
        return 1;
    }

    LARGE_INTEGER pos;
    pos.QuadPart = 0;
    SetFilePointerEx(hFile, pos, nullptr, FILE_END);
    log_msg("[brushsync] streaming console.log output\n");

    char buffer[1024];
    DWORD bytesRead;

    while (1) {
        if (ReadFile(hFile, buffer, sizeof(buffer)-1, &bytesRead, nullptr)) {
            if (bytesRead > 0) {
                buffer[bytesRead] = 0;

                char *line = buffer;
                char *bufend = buffer + bytesRead;

                while (line < bufend) {
                    char *next = strchr(line, '\n');
                    if (next) {
                        *next = '\0';
                    }

                    /* strip trailing carriage return if present */
                    size_t len = strlen(line);
                    if (len > 0 && line[len-1] == '\r') {
                        line[len-1] = '\0';
                    }

                    Int3 start;
                    Int3 end;
                    if (sscanf(line, "[fatalis-movebrushes] move %d %d %d to %d %d %d",
                           &start.x, &start.y, &start.z, &end.x, &end.y, &end.z) == 6) {
                        move_brush(start, end);
                    }

                    if (!next) {
                        break;
                    }
                    line = next + 1;
                }
            }
        }

        Sleep(100);
    }

    CloseHandle(hFile);
}

