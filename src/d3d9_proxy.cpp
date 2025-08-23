// sasrt-deluxe D3D9 Proxy DLL Copyright (C) 2024 kernaltrap8
// This program comes with ABSOLUTELY NO WARRANTY
// This is free software, and you are welcome to redistribute it
// under certain conditions

// d3d9_proxy.cpp - D3D9 proxy functionality
#include "d3d9_proxy.h"
#include "memory_patcher.h"

static HMODULE realD3D9 = NULL;
typedef IDirect3D9* (WINAPI *Direct3DCreate9_t)(UINT);
static Direct3DCreate9_t Real_Direct3DCreate9 = NULL;
extern "C" IMAGE_DOS_HEADER __ImageBase;

void InitializeD3D9Proxy() {
    Log("Initializing D3D9 proxy...\n");
    
    // Load the real system D3D9
    char systemPath[MAX_PATH];
    GetSystemDirectoryA(systemPath, MAX_PATH);
    strcat(systemPath, "\\d3d9.dll");

    realD3D9 = LoadLibraryA(systemPath);
    if (!realD3D9) {
        Log("Failed to load system d3d9.dll\n");
        MessageBoxA(NULL, "Failed to load system d3d9.dll", "Error", MB_OK);
        return;
    }

    Real_Direct3DCreate9 = (Direct3DCreate9_t)GetProcAddress(realD3D9, "Direct3DCreate9");
    if (!Real_Direct3DCreate9) {
        Log("Failed to get Direct3DCreate9 from system d3d9.dll\n");
        MessageBoxA(NULL, "Failed to get Direct3DCreate9 from system d3d9.dll", "Error", MB_OK);
        return;
    }

    Log("D3D9 proxy initialized successfully\n");
}

// Hook for Direct3DCreate9. We run our actual patching code here,
// then return to the real D3D9 function.
extern "C" __declspec(dllexport) IDirect3D9 *WINAPI Direct3DCreate9(UINT sdkVersion) {
    Log("FakeDirect3DCreate9 called with SDK version: %u\n", sdkVersion);
    
    // Run memory patching when D3D9 is initialized
    PatchMemory();
    
    if (!Real_Direct3DCreate9) {
        Log("Error: Real_Direct3DCreate9 is null!\n");
        return nullptr;
    }
    
    return Real_Direct3DCreate9(sdkVersion);
}
