// sasrt-deluxe D3D9 Proxy DLL Copyright (C) 2024 kernaltrap8
// This program comes with ABSOLUTELY NO WARRANTY
// This is free software, and you are welcome to redistribute it
// under certain conditions

// window_hooks.cpp - Window title hooking functionality
#include "window_hooks.h"

// Static variables for window title hooks
static char* customWindowTitle = nullptr;
static wchar_t* customWindowTitleW = nullptr;

// Function pointers for original APIs
typedef BOOL (WINAPI *SetWindowTextA_t)(HWND hWnd, LPCSTR lpString);
typedef BOOL (WINAPI *SetWindowTextW_t)(HWND hWnd, LPCWSTR lpString);
typedef HWND (WINAPI *CreateWindowExA_t)(DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName,
                                          DWORD dwStyle, int X, int Y, int nWidth, int nHeight,
                                          HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam);

static SetWindowTextA_t Real_SetWindowTextA = nullptr;
static SetWindowTextW_t Real_SetWindowTextW = nullptr;
static CreateWindowExA_t Real_CreateWindowExA = nullptr;

// Hook implementations
BOOL WINAPI Fake_SetWindowTextA(HWND hWnd, LPCSTR lpString) {
    if (lpString && customWindowTitle) {
        // Check if this contains our target strings
        if (strstr(lpString, "Sonic & All-Stars Racing Transformed") ||
            strstr(lpString, "ASN") ||
            strstr(lpString, "BUILD (532043")) {
            
            Log("Intercepted SetWindowTextA: \"%s\"\n", lpString);
            Log("Replacing with: \"%s\"\n", customWindowTitle);
            return Real_SetWindowTextA(hWnd, customWindowTitle);
        }
    }
    
    // Pass through other window titles unchanged
    return Real_SetWindowTextA(hWnd, lpString);
}

BOOL WINAPI Fake_SetWindowTextW(HWND hWnd, LPCWSTR lpString) {
    if (lpString && customWindowTitleW) {
        // Convert to narrow string for easier checking
        char narrowString[512];
        WideCharToMultiByte(CP_UTF8, 0, lpString, -1, narrowString, sizeof(narrowString), nullptr, nullptr);
        
        if (strstr(narrowString, "Sonic & All-Stars Racing Transformed") ||
            strstr(narrowString, "ASN") ||
            strstr(narrowString, "BUILD (532043")) {
            
            Log("Intercepted SetWindowTextW: \"%s\"\n", narrowString);
            Log("Replacing with wide string version\n");
            return Real_SetWindowTextW(hWnd, customWindowTitleW);
        }
    }
    
    return Real_SetWindowTextW(hWnd, lpString);
}

HWND WINAPI Fake_CreateWindowExA(DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName,
                                 DWORD dwStyle, int X, int Y, int nWidth, int nHeight,
                                 HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam) {
    
    LPCSTR finalWindowName = lpWindowName;
    
    if (lpWindowName && customWindowTitle &&
        (strstr(lpWindowName, "Sonic & All-Stars Racing Transformed") ||
         strstr(lpWindowName, "ASN") ||
         strstr(lpWindowName, "BUILD (532043"))) {
        
        Log("Intercepted CreateWindowExA with title: \"%s\"\n", lpWindowName);
        Log("Using custom title in CreateWindowExA: \"%s\"\n", customWindowTitle);
        finalWindowName = customWindowTitle;
    }
    
    return Real_CreateWindowExA(dwExStyle, lpClassName, finalWindowName, dwStyle, 
                               X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
}

// Public functions
void SetupWindowTitleHooks(const char* newTitle) {
    // Free any existing custom titles
    if (customWindowTitle) {
        free(customWindowTitle);
        customWindowTitle = nullptr;
    }
    if (customWindowTitleW) {
        free(customWindowTitleW);
        customWindowTitleW = nullptr;
    }
    
    if (!newTitle) {
        Log("SetupWindowTitleHooks: No custom title provided, hooks disabled\n");
        return;
    }
    
    // Store the custom title (narrow string)
    size_t titleLen = strlen(newTitle);
    customWindowTitle = (char*)malloc(titleLen + 1);
    if (customWindowTitle) {
        strcpy(customWindowTitle, newTitle);
    }
    
    // Convert to wide string
    size_t wideLen = titleLen + 1;
    customWindowTitleW = (wchar_t*)malloc(wideLen * sizeof(wchar_t));
    if (customWindowTitleW) {
        MultiByteToWideChar(CP_UTF8, 0, newTitle, -1, customWindowTitleW, wideLen);
    }
    
    Log("Setting up window title hooks with custom title: \"%s\"\n", newTitle);
    
    // Get original functions if not already done
    if (!Real_SetWindowTextA) {
        HMODULE user32 = GetModuleHandleA("user32.dll");
        if (user32) {
            Real_SetWindowTextA = (SetWindowTextA_t)GetProcAddress(user32, "SetWindowTextA");
            Real_SetWindowTextW = (SetWindowTextW_t)GetProcAddress(user32, "SetWindowTextW");
            Real_CreateWindowExA = (CreateWindowExA_t)GetProcAddress(user32, "CreateWindowExA");
        }
    }
    
    // Hook them
    PatchIAT_ReplaceImport("USER32.dll", "SetWindowTextA", (void*)Fake_SetWindowTextA);
    PatchIAT_ReplaceImport("USER32.dll", "SetWindowTextW", (void*)Fake_SetWindowTextW);
    PatchIAT_ReplaceImport("USER32.dll", "CreateWindowExA", (void*)Fake_CreateWindowExA);
}

void DisableWindowTitleHooks() {
    SetupWindowTitleHooks(nullptr);
}
