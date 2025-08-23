// sasrt-deluxe D3D9 Proxy DLL Copyright (C) 2024 kernaltrap8
// This program comes with ABSOLUTELY NO WARRANTY
// This is free software, and you are welcome to redistribute it
// under certain conditions

// d3d9_proxy.h - D3D9 proxy functionality
#ifndef D3D9_PROXY_H
#define D3D9_PROXY_H

#include "common.h"

// D3D9 proxy functions
extern "C" __declspec(dllexport) IDirect3D9 *WINAPI Direct3DCreate9(UINT sdkVersion);
void InitializeD3D9Proxy();

#endif // D3D9_PROXY_H
