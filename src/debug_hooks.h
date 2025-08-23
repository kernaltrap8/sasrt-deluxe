// sasrt-deluxe D3D9 Proxy DLL Copyright (C) 2024 kernaltrap8
// This program comes with ABSOLUTELY NO WARRANTY
// This is free software, and you are welcome to redistribute it
// under certain conditions

// debug_hooks.h - Debug and anti-debug functionality
#ifndef DEBUG_HOOKS_H
#define DEBUG_HOOKS_H

#include "common.h"

// Debug hook functions
BOOL WINAPI FakeIsDebuggerPresent(void);
BOOL WINAPI FakeCheckRemoteDebuggerPresent(HANDLE hProcess, PBOOL pbDebuggerPresent);
void WINAPI FakeOutputDebugStringA(LPCSTR lpOutputString);

#endif // DEBUG_HOOKS_H
