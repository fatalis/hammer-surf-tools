#include "hotkeys.h"

static const ACCEL hotkey1 = { FALT | FSHIFT, 'R', CMD_RAMP_GENERATOR };
static const ACCEL hotkey2 = { FALT | FSHIFT, 'A', CMD_ANGLEFIX};
static const ACCEL hotkey3 = { FALT | FSHIFT, 'T', CMD_TRIGGER_GENERATOR };
static const ACCEL hotkey4 = { FALT | FSHIFT, 'E', CMD_EXTRUDE_FACE };

static const ACCEL hotkeys[] = { hotkey1, hotkey2, hotkey3, hotkey4 };

HACCEL check_add_hotkeys(HACCEL accel, LPCSTR lpTableName) {
    if (lpTableName == MAKEINTRESOURCE(IDR_FORGEMAPTYPE)) {
        int count = CopyAcceleratorTableA(accel, nullptr, 0);
        int extra_hotkeys = ARRAY_LEN(hotkeys);

        LPACCEL new_accel = malloc(sizeof(ACCEL) * (size_t)(count + extra_hotkeys));
        CopyAcceleratorTableA(accel, new_accel, count);

        for (auto i = 0; i < extra_hotkeys; i++) {
            new_accel[count + i] = hotkeys[i];
        }

        // log_msg("hook_LoadAcceleratorsA: %p %d = %d %p\n", hInstance, lpTableName, count, accel);

        accel = CreateAcceleratorTableA(new_accel, count + extra_hotkeys);
        free(new_accel);
    }

    return accel;
}
