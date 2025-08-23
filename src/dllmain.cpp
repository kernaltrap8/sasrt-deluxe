// sasrt-deluxe D3D9 Proxy DLL Copyright (C) 2024 kernaltrap8
// This program comes with ABSOLUTELY NO WARRANTY
// This is free software, and you are welcome to redistribute it
// under certain conditions

// dllmain.cpp - Main DLL entry point
#include "common.h"
#include "d3d9_proxy.h"
#include "debug_hooks.h"
#include "window_hooks.h"

static const char* CUSTOM_WINDOW_TITLE = "Sonic & All-Stars Racing Transformed Deluxe - BUILD (532043 - Jan 15 2014 10:38:42)";

// TODO: Could load these from a config file instead.
// TODO:
// force windowed mode with vsync enabled, vsync off regardless of
// window config will lock the game to 58FPS. Windowed mode uncaps FPS to vblank interval.

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved) {
    switch (fdwReason) {
        case DLL_PROCESS_ATTACH:
            Log("=== D3D9 Proxy DLL Attached ===\n");
            
            // Prevent loader lock issues
            DisableThreadLibraryCalls(hinstDLL);

            // Initialize D3D9 proxy
            InitializeD3D9Proxy();

            // Patch IAT imports in the game
            Log("Setting up API hooks...\n");
            PatchIAT_ReplaceImport("D3D9.DLL", "Direct3DCreate9", (void*)Direct3DCreate9);
            PatchIAT_ReplaceImport("KERNEL32.dll", "IsDebuggerPresent", (void*)FakeIsDebuggerPresent);
            PatchIAT_ReplaceImport("KERNEL32.dll", "OutputDebugStringA", (void*)FakeOutputDebugStringA);
            
            // Set up window title hooks
            SetupWindowTitleHooks(CUSTOM_WINDOW_TITLE);
            
            Log("DLL initialization complete\n");
            break;

        case DLL_PROCESS_DETACH:
            Log("=== D3D9 Proxy DLL Detaching ===\n");
            // Cleanup could go here if needed
            break;
            
        case DLL_THREAD_ATTACH:
        case DLL_THREAD_DETACH:
            // Nothing to do for thread attach/detach
            break;
    }
    return TRUE;
}
