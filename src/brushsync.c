#include "brushsync.h"
#include "hooks.h"
#include "scriptfuncs.h"
#include "hammerfuncs.h"

// moves brushes in hammer when moved ingame with a vscript
// proof of concept, vscript code isn't finished

typedef struct {
    int pos[3];
    CMapClass *ent;
} FindEntity;

static bool find_ent_by_pos(CMapClass *ent, void *param) {
    FindEntity *find = param;

    char *name = ent->vtable->GetType(ent);
    if (!strcmp(name, "CMapEntity")) {
        int rx = (int)roundf(ent->m_Origin.x);
        int ry = (int)roundf(ent->m_Origin.y);
        int rz = (int)roundf(ent->m_Origin.z);

        if (rx == find->pos[0] && ry == find->pos[1] && rz == find->pos[2]) {
            find->ent = ent;
            return false;
        }
    }

    return true;
}

static void move_brush(int *start, int *end) {
    CMapDoc *doc = GetActiveMapDoc();
    if (!doc || !doc->m_pWorld) {
        return;
    }

    FindEntity find;
    find.pos[0] = start[0];
    find.pos[1] = start[1];
    find.pos[2] = start[2];
    find.ent = nullptr;

    CMapClass_EnumChildren(doc->m_pWorld, find_ent_by_pos, &find, nullptr);
    /* log_msg("[hook] FindEnt = %p\n", find.ent); */

    if (find.ent) {
        Vec3 delta;
        TransMove(find.ent, &delta);
        log_msg("[hook] moved %p with delta %.1f %.1f %.1f\n", find.ent, (double)delta.x, (double)delta.y, (double)delta.z);
        CMapDoc_SetModifiedFlag(doc, true);
        CMapDoc_UpdateAllViews(doc, MAPVIEW_UPDATE_OBJECTS, nullptr);
    } else {
        log_msg("[hook] failed to find ent at %d %d %d\n", find.pos[0], find.pos[1], find.pos[2]);
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

                    int x1, y1, z1, x2, y2, z2;
                    if (sscanf(line, "[fatalis-movebrushes] move %d %d %d to %d %d %d",
                           &x1, &y1, &z1, &x2, &y2, &z2) == 6) {

                        int start[3];
                        start[0] = x1;
                        start[1] = y1;
                        start[2] = z1;

                        int end[3];
                        end[0] = x2;
                        end[1] = y2;
                        end[2] = z2;

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

