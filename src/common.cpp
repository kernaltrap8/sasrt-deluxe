// sasrt-deluxe D3D9 Proxy DLL Copyright (C) 2024 kernaltrap8
// This program comes with ABSOLUTELY NO WARRANTY
// This is free software, and you are welcome to redistribute it
// under certain conditions

// common.cpp - Shared utilities and logging
#include "common.h"
#include <stdarg.h>

static bool logStarted = false;

void Log(const char *fmt, ...) {
    FILE *f;
    if (!logStarted) {
        // overwrite the file on the first write
        f = fopen("d3d9_proxy_log.txt", "w");
        logStarted = true;
    } else {
        // append after that
        f = fopen("d3d9_proxy_log.txt", "a");
    }

    if (!f) return;

    va_list ap;
    va_start(ap, fmt);
    vfprintf(f, fmt, ap);
    va_end(ap);
    fclose(f);
}

float GetRefreshRate() {
    DEVMODE dm = {};
    dm.dmSize = sizeof(dm);
    if (EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &dm)) {
        if (dm.dmDisplayFrequency > 0) {
            return (float)dm.dmDisplayFrequency;
        }
    }
    return 60.0f; // fallback
}

BOOL PatchIAT_ReplaceImport(const char *moduleName, const char *importName, void *newFunc) {
    HMODULE hMod = GetModuleHandleA(NULL); // main exe base
    if (!hMod) {
        Log("PatchIAT: GetModuleHandleA(NULL) failed\n");
        return FALSE;
    }

    BYTE *base = (BYTE*)hMod;
    PIMAGE_DOS_HEADER dos = (PIMAGE_DOS_HEADER)base;
    if (dos->e_magic != IMAGE_DOS_SIGNATURE) {
        Log("PatchIAT: bad DOS signature\n");
        return FALSE;
    }

    PIMAGE_NT_HEADERS nt = (PIMAGE_NT_HEADERS)(base + dos->e_lfanew);
    if (nt->Signature != IMAGE_NT_SIGNATURE) {
        Log("PatchIAT: bad NT signature\n");
        return FALSE;
    }

    DWORD importRVA = nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress;
    if (!importRVA) {
        Log("PatchIAT: no import directory\n");
        return FALSE;
    }

    PIMAGE_IMPORT_DESCRIPTOR imp = (PIMAGE_IMPORT_DESCRIPTOR)(base + importRVA);
    for (; imp->Name; ++imp) {
        const char *impModName = (const char*)(base + imp->Name);
        if (_stricmp(impModName, moduleName) != 0) continue;

        PIMAGE_THUNK_DATA oft = (PIMAGE_THUNK_DATA)(base + imp->OriginalFirstThunk);
        PIMAGE_THUNK_DATA ft  = (PIMAGE_THUNK_DATA)(base + imp->FirstThunk);
        if (!oft || !ft) {
            Log("PatchIAT: bad thunks\n");
            return FALSE;
        }

        for (; oft->u1.AddressOfData; ++oft, ++ft) {
            if (IMAGE_SNAP_BY_ORDINAL(oft->u1.Ordinal)) continue;

            PIMAGE_IMPORT_BY_NAME ibn = (PIMAGE_IMPORT_BY_NAME)(base + oft->u1.AddressOfData);
            const char *name = (const char*)ibn->Name;
            if (!name) continue;

            if (strcmp(name, importName) == 0) {
                DWORD oldProtect;
                PROC *entry = (PROC*)&ft->u1.Function;
                if (!VirtualProtect(entry, sizeof(void*), PAGE_EXECUTE_READWRITE, &oldProtect)) {
                    Log("PatchIAT: VirtualProtect failed for %s!%s\n", impModName, name);
                    return FALSE;
                }
                Log("PatchIAT: replacing import %s!%s (old=%p new=%p)\n", impModName, name, (void*)*entry, newFunc);
                *entry = (PROC)newFunc;
                VirtualProtect(entry, sizeof(void*), oldProtect, &oldProtect);
                return TRUE;
            }
        }
    }

    Log("PatchIAT: did not find %s!%s\n", moduleName, importName);
    return FALSE;
}
