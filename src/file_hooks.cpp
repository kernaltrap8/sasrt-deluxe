// sasrt-deluxe D3D9 Proxy DLL Copyright (C) 2024 kernaltrap8
// This program comes with ABSOLUTELY NO WARRANTY
// This is free software, and you are welcome to redistribute it
// under certain conditions

// file_hooks.cpp - Hooks for file related Windows API calls

#include "file_hooks.h"
#include <map>
#include <string>
#include <windows.h>

// --- Track handle → filename ---
static std::map<HANDLE, std::string> g_FileMap;

// --- Hooked functions ---
HANDLE WINAPI Hooked_CreateFileA(
    LPCSTR lpFileName, DWORD access, DWORD share,
    LPSECURITY_ATTRIBUTES sa, DWORD disposition,
    DWORD flags, HANDLE templateFile
) {
    HANDLE h = Real_CreateFileA(lpFileName, access, share, sa, disposition, flags, templateFile);
    if (h != INVALID_HANDLE_VALUE) {
        g_FileMap[h] = lpFileName ? lpFileName : "";
    }
    Log("[Hook] CreateFileA: %s -> handle=%p (from %p)\n",
        lpFileName, h, _ReturnAddress());
    return h;
}

HANDLE WINAPI Hooked_CreateFileW(
    LPCWSTR lpFileName, DWORD access, DWORD share,
    LPSECURITY_ATTRIBUTES sa, DWORD disposition,
    DWORD flags, HANDLE templateFile
) {
    HANDLE h = Real_CreateFileW(lpFileName, access, share, sa, disposition, flags, templateFile);
    if (h != INVALID_HANDLE_VALUE) {
        // Convert wide string to UTF-8 for logging
        char utf8[MAX_PATH];
        WideCharToMultiByte(CP_UTF8, 0, lpFileName, -1, utf8, MAX_PATH, NULL, NULL);
        g_FileMap[h] = utf8;
        Log("[Hook] CreateFileW: %s -> handle=%p (from %p)\n",
            utf8, h, _ReturnAddress());
    }
    return h;
}

BOOL WINAPI Hooked_ReadFile(
    HANDLE hFile, LPVOID buffer, DWORD toRead,
    LPDWORD read, LPOVERLAPPED overlapped
) {
    BOOL ok = Real_ReadFile(hFile, buffer, toRead, read, overlapped);
    auto it = g_FileMap.find(hFile);
    const char* name = (it != g_FileMap.end()) ? it->second.c_str() : "<unknown>";
    Log("[Hook] ReadFile: %s handle=%p, bytes=%u (from %p)\n",
        name, hFile, toRead, _ReturnAddress());
    return ok;
}

BOOL WINAPI Hooked_WriteFile(
    HANDLE hFile, LPCVOID buffer, DWORD toWrite,
    LPDWORD written, LPOVERLAPPED overlapped
) {
    BOOL ok = Real_WriteFile(hFile, buffer, toWrite, written, overlapped);
    auto it = g_FileMap.find(hFile);
    const char* name = (it != g_FileMap.end()) ? it->second.c_str() : "<unknown>";
    Log("[Hook] WriteFile: %s handle=%p, bytes=%u (from %p)\n",
        name, hFile, toWrite, _ReturnAddress());
    return ok;
}

BOOL WINAPI Hooked_CloseHandle(HANDLE hObject) {
    auto it = g_FileMap.find(hObject);
    if (it != g_FileMap.end()) {
        Log("[Hook] CloseHandle: %s handle=%p (from %p)\n",
            it->second.c_str(), hObject, _ReturnAddress());
        g_FileMap.erase(it);
    } else {
        Log("[Hook] CloseHandle: handle=%p (from %p)\n",
            hObject, _ReturnAddress());
    }
    return Real_CloseHandle(hObject);
}
