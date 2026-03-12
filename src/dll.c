#include "wrapper.h"
#include "patterns.h"
#include "scriptfuncs.h"

static DWORD WINAPI hook_init_thread(LPVOID param) {
    log_msg("[hook] DLL loaded\n");

    dump_offsets();
    wrapper();

    HMODULE mod;
    while (!(mod = GetModuleHandleA("hammerplusplus_dll.dll")))
        Sleep(100);

    if (MH_Initialize() != MH_OK) {
        log_msg("[hook] MH_Initialize failed\n");
        return 1;
    }

    MODULEINFO mi;
    GetModuleInformation(GetCurrentProcess(), mod, &mi, sizeof(mi));

    uint8_t *base = (uint8_t *)mi.lpBaseOfDll;
    size_t size = mi.SizeOfImage;

    if (!scan_all(base, size)) {
        log_msg("[hook] error: at least one required pattern wasn't found\n");
        return 1;
    }

    if (!script_data_init()) {
        log_msg("[hook] at least one required script func wasn't found\n");
        return 1;
    }

    if (MH_EnableHook(MH_ALL_HOOKS) != MH_OK) {
        log_msg("[hook] error: MH_EnableHook failed\n");
        MH_DisableHook(MH_ALL_HOOKS);
        return 1;
    }

    log_msg("[hook] setup hooks successfully\n");

    return 0;
}

BOOL WINAPI DllMain(HINSTANCE hinst, DWORD reason, LPVOID) {
    if (reason == DLL_PROCESS_ATTACH) {
        HANDLE thread;

        DisableThreadLibraryCalls(hinst);

        thread = CreateThread(nullptr, 0, hook_init_thread, hinst, 0, nullptr);

        if (thread) {
            CloseHandle(thread);
        } else {
            log_msg("[hook] failed to create init thread (err=%lu)\n",
                    GetLastError());
        }
    }

    return TRUE;
}
