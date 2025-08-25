// sasrt-deluxe D3D9 Proxy DLL Copyright (C) 2024 kernaltrap8
// This program comes with ABSOLUTELY NO WARRANTY
// This is free software, and you are welcome to redistribute it
// under certain conditions

// file_hooks.h - Definitions for file_hooks.cpp

#ifndef FILE_HOOKS_H
#define FILE_HOOKS_H
#include "common.h"
#define _ReturnAddress() __builtin_return_address(0)

// --- Real function pointers ---
static HANDLE (WINAPI *Real_CreateFileA)(
    LPCSTR, DWORD, DWORD, LPSECURITY_ATTRIBUTES, DWORD, DWORD, HANDLE
) = CreateFileA;

static HANDLE (WINAPI *Real_CreateFileW)(
    LPCWSTR, DWORD, DWORD, LPSECURITY_ATTRIBUTES, DWORD, DWORD, HANDLE
) = CreateFileW;

static BOOL (WINAPI *Real_ReadFile)(
    HANDLE, LPVOID, DWORD, LPDWORD, LPOVERLAPPED
) = ReadFile;

static BOOL (WINAPI *Real_WriteFile)(
    HANDLE, LPCVOID, DWORD, LPDWORD, LPOVERLAPPED
) = WriteFile;

static BOOL (WINAPI *Real_CloseHandle)(
    HANDLE
) = CloseHandle;

// --- Hooked function definitons ---
HANDLE WINAPI Hooked_CreateFileA(
    LPCSTR lpFileName, DWORD access, DWORD share,
    LPSECURITY_ATTRIBUTES sa, DWORD disposition,
    DWORD flags, HANDLE templateFile
);

HANDLE WINAPI Hooked_CreateFileW(
    LPCWSTR lpFileName, DWORD access, DWORD share,
    LPSECURITY_ATTRIBUTES sa, DWORD disposition,
    DWORD flags, HANDLE templateFile
);

BOOL WINAPI Hooked_ReadFile(
    HANDLE hFile, LPVOID buffer, DWORD toRead,
    LPDWORD read, LPOVERLAPPED overlapped
);

BOOL WINAPI Hooked_WriteFile(
    HANDLE hFile, LPCVOID buffer, DWORD toWrite,
    LPDWORD written, LPOVERLAPPED overlapped
);

BOOL WINAPI Hooked_CloseHandle(HANDLE hObject);

#endif // FILE_HOOKS_H