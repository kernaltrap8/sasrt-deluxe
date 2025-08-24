// sasrt-deluxe D3D9 Proxy DLL Copyright (C) 2024 kernaltrap8
// This program comes with ABSOLUTELY NO WARRANTY
// This is free software, and you are welcome to redistribute it
// under certain conditions

// memory_patcher.cpp - Memory patching functionality
#include "memory_patcher.h"

// Configuration constants
const float originalFloatValue = 1.0f / 60.0f;
//const float newFloatValue = 1.0f / 100.0f;

// Static variables
static bool alreadyPatched = false;
static int patchCount = 0;

// Game-specific offsets
// TODO: add offsets for older versions (and possibly patched steamstub execs)
// Sonic & All-Stars Racing Transformed BUILD (532043 - Jan 15 2014 10:38:42)
static uintptr_t frameTiming = 0x660528;
static uintptr_t menuPhysicsTiming = 0x6622ac;
static uintptr_t vehiclePhysicsTiming = 0x6b3ba0;

// address(es) to skip
static const uintptr_t skipOffsets[] = { vehiclePhysicsTiming, menuPhysicsTiming, frameTiming };
static const size_t skipCount = sizeof(skipOffsets) / sizeof(skipOffsets[0]);

bool IsTargetFloat(float value) {
    return fabs(value - originalFloatValue) < 0.0000001f;
}

bool ShouldSkip(uintptr_t offset) {
    for (size_t i = 0; i < skipCount; i++) {
        if (offset == skipOffsets[i]) return true;
    }
    return false;
}

void PatchMemory() {
    if (alreadyPatched) return;

    HMODULE baseModule = GetModuleHandle(NULL);
    if (!baseModule) return;

    MODULEINFO modInfo;
    if (!GetModuleInformation(GetCurrentProcess(), baseModule, &modInfo, sizeof(modInfo))) return;

    unsigned char* addr = (unsigned char*)modInfo.lpBaseOfDll;
    unsigned char* endAddr = addr + modInfo.SizeOfImage;
    MEMORY_BASIC_INFORMATION mbi;

    double refreshDouble = (double)GetRefreshRate();
    double tickDouble = 1.0 / GetRefreshRate();
    int doublePatched = 0;
    int tickPatched = 0;

    while (addr < endAddr && (doublePatched < 1 || tickPatched < 1)) {
        if (VirtualQuery(addr, &mbi, sizeof(mbi)) == sizeof(mbi)) {
            if ((mbi.State == MEM_COMMIT) &&
                (mbi.Protect & (PAGE_READWRITE | PAGE_READONLY | PAGE_EXECUTE_READ)) &&
                !(mbi.Protect & (PAGE_GUARD | PAGE_NOACCESS))) {

                unsigned char* regionBase = (unsigned char*)mbi.BaseAddress;
                size_t regionSize = mbi.RegionSize;

                for (size_t i = 0; i < regionSize; i++) {
                    unsigned char* p = regionBase + i;

                    // Patch 8-byte double refresh rate
                    if (doublePatched < 1 && i + 8 <= regionSize) {
                        double val;
                        memcpy(&val, p, sizeof(double));
                        if (fabs(val - 60.0) < 1e-6) {
                            DWORD oldProtect;
                            VirtualProtect(p, 8, PAGE_READWRITE, &oldProtect);
                            memcpy(p, &refreshDouble, 8);
                            VirtualProtect(p, 8, oldProtect, &oldProtect);
                            doublePatched++;
                            Log("[Double FPS] Patched at %p -> %f\n", p, refreshDouble);
                        }
                    }

                    // Patch 8-byte tick/delta
                    if (tickPatched < 1 && i + 8 <= regionSize) {
                        double val;
                        memcpy(&val, p, sizeof(double));
                        if (fabs(val - 0.01666666753590107) < 1e-9) {
                            DWORD oldProtect;
                            VirtualProtect(p, 8, PAGE_READWRITE, &oldProtect);
                            memcpy(p, &tickDouble, 8);
                            VirtualProtect(p, 8, oldProtect, &oldProtect);
                            tickPatched++;
                            Log("[Tick double] Patched at %p -> %f\n", p, tickDouble);
                        }
                    }
                }
            }
            addr += mbi.RegionSize;
        } else break;
    }

    Log("PatchMemory complete: Double FPS patched=%d, Tick double patched=%d\n", doublePatched, tickPatched);
    alreadyPatched = true;
}

