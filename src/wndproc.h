#ifndef WNDPROC_H
#define WNDPROC_H

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "common.h"

typedef LRESULT (*AfxWndProc_t)(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
extern AfxWndProc_t orig_AfxWndProc;
LRESULT hook_AfxWndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);

#endif // WNDPROC_H
