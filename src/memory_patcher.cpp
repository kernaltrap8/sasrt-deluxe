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
static const uintptr_t skipOffsets[] = { vehiclePhysicsTiming };
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

    float dynamicFloatValue = GetRefreshRate();
    Log("dynamicFloatValue : %f\n", dynamicFloatValue);

    HMODULE hModule = GetModuleHandle(NULL);
    if (!hModule) return;

    MODULEINFO modInfo;
    if (!GetModuleInformation(GetCurrentProcess(), hModule, &modInfo, sizeof(modInfo))) return;

    unsigned char *baseAddr = (unsigned char *)modInfo.lpBaseOfDll;
    unsigned char *endAddr = baseAddr + modInfo.SizeOfImage;
    unsigned char *addr = baseAddr;
    MEMORY_BASIC_INFORMATION mbi;

    Log("Starting memory patching...\n");

    while (addr < endAddr && patchCount < 2) {
        if (VirtualQuery(addr, &mbi, sizeof(mbi)) == sizeof(mbi)) {
            if ((mbi.State == MEM_COMMIT) &&
                (mbi.Protect & (PAGE_READWRITE | PAGE_READONLY | PAGE_EXECUTE_READ)) &&
                !(mbi.Protect & (PAGE_GUARD | PAGE_NOACCESS))) {

                float *f = (float *)(((uintptr_t)addr + 3) & ~3);
                float *endF = (float *)((unsigned char *)mbi.BaseAddress + mbi.RegionSize);

                while (f < endF && patchCount < 2) {
                    if ((unsigned char *)f + sizeof(float) > (unsigned char *)mbi.BaseAddress + mbi.RegionSize) break;

                    if (IsTargetFloat(*f)) {
                        uintptr_t currentOffset = (unsigned char *)f - baseAddr;
                        if (ShouldSkip(currentOffset)) { f++; continue; }

                        patchCount++;
                        Log("Found float #%d at 0x%p, patching %.8f -> %.8f\n",
                            patchCount, f, *f, dynamicFloatValue);

                        DWORD oldProtect;
                        if (VirtualProtect(f, sizeof(float), PAGE_READWRITE, &oldProtect)) {
                            *f = dynamicFloatValue;
                            VirtualProtect(f, sizeof(float), oldProtect, &oldProtect);
                            Log("Successfully patched float #%d!\n", patchCount);
                        }
                    }
                    f++;
                }
            }
            addr += mbi.RegionSize;
        } else { break; }
    }

    Log("Finished patching floats (patched %d floats)\n", patchCount);
    alreadyPatched = true;
}

