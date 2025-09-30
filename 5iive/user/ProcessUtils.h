#pragma once
#ifndef PROCESS_UTILS_H
#define PROCESS_UTILS_H

#include <windows.h>
#include <string>
#include <vector>
#include <tlhelp32.h>
#include <psapi.h>

namespace ProcessUtils {
    // Basic process operations
    DWORD FindProcessId(const std::wstring& processName);
    std::vector<DWORD> FindProcessIds(const std::wstring& processName);
    bool IsProcessRunning(DWORD pid);
    std::wstring GetProcessName(DWORD pid);
    
    // Module operations
    ULONG_PTR GetModuleBaseAddress(DWORD pid, const std::wstring& moduleName = L"");
    HMODULE GetModuleHandle(DWORD pid, const std::wstring& moduleName);
    
    // Process information
    bool Is64BitProcess(DWORD pid);
    DWORD GetParentProcessId(DWORD pid);
    bool HasWindowTitle(DWORD pid, const std::wstring& windowTitle);
    
    // Utility functions
    DWORD WaitForProcess(const std::wstring& processName, DWORD timeoutMs = 30000);
    bool IsProcessElevated(DWORD pid);

} // namespace ProcessUtils

#endif // PROCESS_UTILS_H