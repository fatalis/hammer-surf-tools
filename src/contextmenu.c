#include "contextmenu.h"
#include "triggergen.h"
#include "anglefix.h"
#include "rampgen.h"

static void add_menus(HMENU hPopup, int pos) {
    HMENU hSubMenu = CreatePopupMenu();
    AppendMenuA(hSubMenu, MF_STRING, CMD_CURVED_RAMP_GENERATOR, "Curved Ramp Generator\tCtrl+Alt+Shift+S");
    AppendMenuA(hSubMenu, MF_STRING, CMD_ANGLEFIX,              "Anglefix\tCtrl+Alt+Shift+A");
    AppendMenuA(hSubMenu, MF_STRING, CMD_TRIGGER_GENERATOR,     "Trigger Generator\tCtrl+Alt+Shift+T");

    InsertMenuA(
        hPopup,
        (uint32_t)pos,
        MF_BYPOSITION | MF_POPUP,
        (UINT_PTR)hSubMenu,
        "Surf Tools"
    );
}


UINT decide_menu_item_enabled(HMENU hMenu, UINT uIDEnableItem, UINT uEnable) {
    if (uEnable & MF_BYPOSITION) {
        UINT id = GetMenuItemID(hMenu, (int)uIDEnableItem);
        if (id == CMD_CURVED_RAMP_GENERATOR || id == CMD_ANGLEFIX || id == CMD_TRIGGER_GENERATOR) {
            return MF_ENABLED;
        }
    }
    return uEnable;
}

void menu_check_command(UINT cmd) {
    if (cmd == CMD_CURVED_RAMP_GENERATOR) {
        do_ramp_generator();
    } else if (cmd == CMD_ANGLEFIX) {
        do_anglefix();
    } else if (cmd == CMD_TRIGGER_GENERATOR) {
        do_trigger_generator();
    }
}

void check_add_menus(HMENU menu, const void *lpMenuName) {
    if (lpMenuName == (void *)IDR_POPUPS) {
        HMENU hPopup = GetSubMenu(menu, 0); // "Object"
        int count = GetMenuItemCount(hPopup);
        int second_last = count - 1;
        add_menus(hPopup, second_last);
        InsertMenuA(hPopup, (uint32_t)second_last + 1, MF_BYPOSITION | MF_SEPARATOR, 0, nullptr);
    } else if (lpMenuName == (void *)IDR_FORGEMAPTYPE) {
        HMENU hPopup = GetSubMenu(menu, 4); // "Tools"
        int count = GetMenuItemCount(hPopup);
        add_menus(hPopup, count - 3);
    }
}
