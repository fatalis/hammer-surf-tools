#ifndef CONTEXTMENU_H
#define CONTEXTMENU_H

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

typedef HMENU (*LoadMenuW_t)(HINSTANCE hInstance, LPCWSTR lpMenuName);
extern LoadMenuW_t orig_LoadMenuW;
HMENU hook_LoadMenuW(HINSTANCE hInstance, LPCWSTR lpMenuName);

#endif // CONTEXTMENU_H
