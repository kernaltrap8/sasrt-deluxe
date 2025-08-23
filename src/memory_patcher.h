// sasrt-deluxe D3D9 Proxy DLL Copyright (C) 2024 kernaltrap8
// This program comes with ABSOLUTELY NO WARRANTY
// This is free software, and you are welcome to redistribute it
// under certain conditions

// memory_patcher.h - Memory patching functionality
#ifndef MEMORY_PATCHER_H
#define MEMORY_PATCHER_H

#include "common.h"

// Configuration constants
extern const float originalFloatValue;
extern const float newFloatValue;

// Patching functions
void PatchMemory();
bool IsTargetFloat(float value);
bool ShouldSkip(uintptr_t offset);

#endif // MEMORY_PATCHER_H
