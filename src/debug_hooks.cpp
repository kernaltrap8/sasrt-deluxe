// sasrt-deluxe D3D9 Proxy DLL Copyright (C) 2024 kernaltrap8
// This program comes with ABSOLUTELY NO WARRANTY
// This is free software, and you are welcome to redistribute it
// under certain conditions

// debug_hooks.cpp - Debug and anti-debug functionality
#include "debug_hooks.h"

static int welcomeMessageCounter = 0;

BOOL WINAPI FakeIsDebuggerPresent(void) {
    Log("FakeIsDebuggerPresent called - returning FALSE\n");
    return FALSE;
}

BOOL WINAPI FakeCheckRemoteDebuggerPresent(HANDLE hProcess, PBOOL pbDebuggerPresent) {
    Log("FakeCheckRemoteDebuggerPresent called - returning FALSE\n");
    if (pbDebuggerPresent) *pbDebuggerPresent = FALSE;
    return TRUE;
}

void WINAPI FakeOutputDebugStringA(LPCSTR lpOutputString) {
    if (lpOutputString) {
        if (welcomeMessageCounter == 0) {
            Log("[ODS] Proxy DLL loaded.\n");
            welcomeMessageCounter = 1;
        }
        Log("[ODS] %s", lpOutputString);
    }
}
