#ifndef CONTEXTMENU_H
#define CONTEXTMENU_H

UINT decide_menu_item_enabled(HMENU hMenu, UINT uIDEnableItem, UINT uEnable);
void menu_check_command(UINT cmd);
void check_add_menus(HMENU menu, const void *lpMenuName);

#endif // CONTEXTMENU_H
