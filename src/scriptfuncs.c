#include "common.h"
#include "scriptfuncs.h"

TransScale_t TransScale;
TransMove_t TransMove;
TransRotate_t TransRotate;

void **ScriptDataPtr;

bool script_data_init() {
    assert(ScriptDataPtr);
    void *script_data = *ScriptDataPtr;
    size_t n_script_funcs = *(size_t *)((void*)ScriptDataPtr + SCRIPT_DATA_ARRAY_SIZE_OFFSET);

    for (auto i = 0; i < n_script_funcs; i++) {
        void *script_data_item = script_data + (i * SCRIPT_DATA_SIZE);
        char *name = *(char **)(script_data_item + SCRIPT_DATA_OFFSET_NAME);
        void *func = *(void **)(script_data_item + SCRIPT_DATA_OFFSET_FUNC);

        if (!strcmp(name, "Scale")) {
            TransScale = func;
        }
        else if (!strcmp(name, "Translate")) {
            TransMove = func;
        }
        else if (!strcmp(name, "Rotate")) {
            TransRotate = func;
        }

        log_msg("[scriptfuncs] %-25s found @ %p\n", name, func);
    }

    return TransScale && TransMove && TransRotate;
}
