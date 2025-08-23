// sasrt-deluxe D3D9 Proxy DLL Copyright (C) 2024 kernaltrap8
// This program comes with ABSOLUTELY NO WARRANTY
// This is free software, and you are welcome to redistribute it
// under certain conditions

// common.h - Shared definitions and includes
#ifndef COMMON_H
#define COMMON_H

#include <d3d9.h>
#include <math.h>
#include <psapi.h>
#include <stdio.h>
#include <windows.h>
#include <imagehlp.h>

// Logging function - available everywhere
void Log(const char *fmt, ...);

float GetRefreshRate();

// IAT patching utility
BOOL PatchIAT_ReplaceImport(const char *moduleName, const char *importName, void *newFunc);

#endif // COMMON_H
