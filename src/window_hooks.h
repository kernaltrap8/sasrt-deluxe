// sasrt-deluxe D3D9 Proxy DLL Copyright (C) 2024 kernaltrap8
// This program comes with ABSOLUTELY NO WARRANTY
// This is free software, and you are welcome to redistribute it
// under certain conditions

// window_hooks.h - Window title hooking
#ifndef WINDOW_HOOKS_H
#define WINDOW_HOOKS_H

#include "common.h"

// Window title hook functions
void SetupWindowTitleHooks(const char* newTitle);
void DisableWindowTitleHooks();

// Hook function declarations (for internal use)
BOOL WINAPI Fake_SetWindowTextA(HWND hWnd, LPCSTR lpString);
BOOL WINAPI Fake_SetWindowTextW(HWND hWnd, LPCWSTR lpString);
HWND WINAPI Fake_CreateWindowExA(DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName,
                                 DWORD dwStyle, int X, int Y, int nWidth, int nHeight,
                                 HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam);

#endif // WINDOW_HOOKS_H
