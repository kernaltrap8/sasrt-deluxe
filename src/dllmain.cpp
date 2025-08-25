// sasrt-deluxe D3D9 Proxy DLL Copyright (C) 2024 kernaltrap8
// This program comes with ABSOLUTELY NO WARRANTY
// This is free software, and you are welcome to redistribute it
// under certain conditions

// dllmain.cpp - Main DLL entry point
#include "common.h"
#include "d3d9_proxy.h"
#include "debug_hooks.h"
#include "window_hooks.h"
#include "file_hooks.h"
#include "version.h"

static const char* CUSTOM_WINDOW_TITLE = "Sonic & All-Stars Racing Transformed Deluxe - BUILD (532043 - Jan 15 2014 10:38:42)";

// TODO: Could load these from a config file instead.
// TODO:
// force windowed mode with vsync enabled, vsync off regardless of
// window config will lock the game to 58FPS. Windowed mode uncaps FPS to vblank interval.

bool DumpProcessImage(const char* outPath) {
    HMODULE hMod = GetModuleHandle(NULL); // base of main exe
    if (!hMod) return false;

    auto* dos = (IMAGE_DOS_HEADER*)hMod;
    auto* nt  = (IMAGE_NT_HEADERS*)((BYTE*)hMod + dos->e_lfanew);

    DWORD sizeOfImage = nt->OptionalHeader.SizeOfImage;
    DWORD sizeOfHeaders = nt->OptionalHeader.SizeOfHeaders;

    // Allocate file buffer
    BYTE* dumpBuf = (BYTE*)calloc(1, sizeOfImage);
    if (!dumpBuf) return false;

    // Copy headers
    memcpy(dumpBuf, dos, sizeOfHeaders);

    // Copy each section as loaded
    auto* sec = (IMAGE_SECTION_HEADER*)((BYTE*)&nt->OptionalHeader + nt->FileHeader.SizeOfOptionalHeader);
    for (WORD i = 0; i < nt->FileHeader.NumberOfSections; i++, sec++) {
        BYTE* src = (BYTE*)hMod + sec->VirtualAddress;
        BYTE* dst = dumpBuf + sec->VirtualAddress;
        memcpy(dst, src, sec->Misc.VirtualSize);
    }

    // Write to file
    FILE* f = fopen(outPath, "wb");
    if (!f) {
        free(dumpBuf);
        return false;
    }
    fwrite(dumpBuf, 1, sizeOfImage, f);
    fclose(f);

    free(dumpBuf);
    return true;
}

DWORD WINAPI DumperThread(LPVOID) {
    Log("Sleeping before dumping decrypted PE...\n");
    Sleep(10000);
    Log("Done sleeping.\n");
    char exePath[MAX_PATH];
    GetModuleFileNameA(NULL, exePath, MAX_PATH);

    // strip to directory
    char *lastSlash = strrchr(exePath, '\\');
    if (lastSlash) {
        *(lastSlash + 1) = '\0'; // keep the trailing slash
    }

    // build dump path relative to exe
    char dumpPath[MAX_PATH];
    snprintf(dumpPath, MAX_PATH, "%sdumped.exe", exePath);

    Log("DUMPING DECRYPTED PE TO: %s\n", dumpPath);

    DumpProcessImage(dumpPath);
    return 0;
}

DWORD WINAPI TickWatchThread(LPVOID lpParam) {
    HMODULE baseModule = GetModuleHandle(NULL);
    if (!baseModule) return -1;

    float tickFloat = 1.0f / (GetRefreshRate() - 20.0f);

    // 3 floats to patch at boot
    unsigned char* floatAddrs[3] = {
        (unsigned char*)((uintptr_t)baseModule + 0x660528),
        (unsigned char*)((uintptr_t)baseModule + 0x6622ac),
        (unsigned char*)((uintptr_t)baseModule + 0x6b3ba0)
    };

    for (int i = 0; i < 3; i++) {
        DWORD oldProtect;
        if (VirtualProtect(floatAddrs[i], sizeof(float), PAGE_READWRITE, &oldProtect)) {
            *(float*)floatAddrs[i] = tickFloat;
            VirtualProtect(floatAddrs[i], sizeof(float), oldProtect, &oldProtect);
            Log("[FPS float] Patched float #%d at %p -> %f", i + 1, floatAddrs[i], tickFloat);
        }
    }

    return 0;
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved) {
    switch (fdwReason) {
        case DLL_PROCESS_ATTACH:
            Log("Sonic & All-Stars Racing Transformed Deluxe loaded\n Version %s\n", GIT_TAG);
            Log("=== D3D9 Proxy DLL Attached ===\n");
            
            // Prevent loader lock issues
            DisableThreadLibraryCalls(hinstDLL);

            // Initialize D3D9 proxy
            InitializeD3D9Proxy();

            // Patch IAT imports in the game
            Log("Setting up API hooks...\n");
            PatchIAT_ReplaceImport("D3D9.DLL",     "Direct3DCreate9",    (void*)Direct3DCreate9);
            PatchIAT_ReplaceImport("KERNEL32.dll", "IsDebuggerPresent",  (void*)FakeIsDebuggerPresent);
            PatchIAT_ReplaceImport("KERNEL32.dll", "OutputDebugStringA", (void*)FakeOutputDebugStringA);
            PatchIAT_ReplaceImport("KERNEL32.dll", "CreateFileA",        (void*)Hooked_CreateFileA);
            PatchIAT_ReplaceImport("KERNEL32.dll", "CreateFileW",        (void*)Hooked_CreateFileW);
            PatchIAT_ReplaceImport("KERNEL32.dll", "ReadFile",           (void*)Hooked_ReadFile);
            PatchIAT_ReplaceImport("KERNEL32.dll", "WriteFile",          (void*)Hooked_WriteFile);
            PatchIAT_ReplaceImport("KERNEL32.dll", "CloseHandle",        (void*)Hooked_CloseHandle);

            // Set up window title hooks
            SetupWindowTitleHooks(CUSTOM_WINDOW_TITLE);

            Log("CreateThread(NULL, 0, TickWatchThread, NULL, 0, NULL);\n");

            CreateThread(NULL, 0, TickWatchThread, NULL, 0, NULL);
            
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
