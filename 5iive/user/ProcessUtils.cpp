#include "ProcessUtils.h"
#include <iostream>
#include <chrono>
#include <thread>

namespace ProcessUtils {

    DWORD FindProcessId(const std::wstring& processName) {
        HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if (hSnapshot == INVALID_HANDLE_VALUE) {
            return 0;
        }

        PROCESSENTRY32W pe32;
        pe32.dwSize = sizeof(PROCESSENTRY32W);

        if (!Process32FirstW(hSnapshot, &pe32)) {
            CloseHandle(hSnapshot);
            return 0;
        }

        do {
            if (wcscmp(pe32.szExeFile, processName.c_str()) == 0) {
                CloseHandle(hSnapshot);
                return pe32.th32ProcessID;
            }
        } while (Process32NextW(hSnapshot, &pe32));

        CloseHandle(hSnapshot);
        return 0;
    }

    std::vector<DWORD> FindProcessIds(const std::wstring& processName) {
        std::vector<DWORD> pids;
        HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

        if (hSnapshot == INVALID_HANDLE_VALUE) {
            return pids;
        }

        PROCESSENTRY32W pe32;
        pe32.dwSize = sizeof(PROCESSENTRY32W);

        if (!Process32FirstW(hSnapshot, &pe32)) {
            CloseHandle(hSnapshot);
            return pids;
        }

        do {
            if (wcscmp(pe32.szExeFile, processName.c_str()) == 0) {
                pids.push_back(pe32.th32ProcessID);
            }
        } while (Process32NextW(hSnapshot, &pe32));

        CloseHandle(hSnapshot);
        return pids;
    }

    bool IsProcessRunning(DWORD pid) {
        HANDLE hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
        if (hProcess) {
            DWORD exitCode;
            if (GetExitCodeProcess(hProcess, &exitCode)) {
                CloseHandle(hProcess);
                return (exitCode == STILL_ACTIVE);
            }
            CloseHandle(hProcess);
        }
        return false;
    }

    std::wstring GetProcessName(DWORD pid) {
        HANDLE hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION | PROCESS_VM_READ, FALSE, pid);
        if (hProcess) {
            wchar_t buffer[MAX_PATH];
            DWORD size = MAX_PATH;
            if (QueryFullProcessImageNameW(hProcess, 0, buffer, &size)) {
                CloseHandle(hProcess);
                // Extract just the executable name
                std::wstring fullPath(buffer);
                size_t lastSlash = fullPath.find_last_of(L"\\/");
                if (lastSlash != std::wstring::npos) {
                    return fullPath.substr(lastSlash + 1);
                }
                return fullPath;
            }
            CloseHandle(hProcess);
        }
        return L"";
    }

    ULONG_PTR GetModuleBaseAddress(DWORD pid, const std::wstring& moduleName) {
        HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, pid);
        if (hSnapshot == INVALID_HANDLE_VALUE) {
            return 0;
        }

        MODULEENTRY32W me32;
        me32.dwSize = sizeof(MODULEENTRY32W);

        ULONG_PTR baseAddress = 0;

        if (Module32FirstW(hSnapshot, &me32)) {
            do {
                if (moduleName.empty() || _wcsicmp(me32.szModule, moduleName.c_str()) == 0) {
                    baseAddress = (ULONG_PTR)me32.modBaseAddr;
                    break;
                }
            } while (Module32NextW(hSnapshot, &me32));
        }

        CloseHandle(hSnapshot);
        return baseAddress;
    }

    HMODULE GetModuleHandle(DWORD pid, const std::wstring& moduleName) {
        return (HMODULE)GetModuleBaseAddress(pid, moduleName);
    }

    bool Is64BitProcess(DWORD pid) {
        HANDLE hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
        if (!hProcess) {
            return false;
        }

        BOOL isWow64 = FALSE;
        if (!IsWow64Process(hProcess, &isWow64)) {
            CloseHandle(hProcess);
            return false;
        }

        CloseHandle(hProcess);

        SYSTEM_INFO systemInfo;
        GetNativeSystemInfo(&systemInfo);

        // If we're on 64-bit Windows and it's not WOW64, it's 64-bit
        if (systemInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64 ||
            systemInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_ARM64) {
            return !isWow64;
        }

        return false; // 32-bit system
    }

    DWORD GetParentProcessId(DWORD pid) {
        HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if (hSnapshot == INVALID_HANDLE_VALUE) {
            return 0;
        }

        PROCESSENTRY32W pe32;
        pe32.dwSize = sizeof(PROCESSENTRY32W);

        DWORD parentPid = 0;

        if (Process32FirstW(hSnapshot, &pe32)) {
            do {
                if (pe32.th32ProcessID == pid) {
                    parentPid = pe32.th32ParentProcessID;
                    break;
                }
            } while (Process32NextW(hSnapshot, &pe32));
        }

        CloseHandle(hSnapshot);
        return parentPid;
    }

    bool HasWindowTitle(DWORD pid, const std::wstring& windowTitle) {
        struct WindowData {
            DWORD pid;
            std::wstring targetTitle;
            bool found;
        };

        WindowData data = { pid, windowTitle, false };

        EnumWindows([](HWND hwnd, LPARAM lParam) -> BOOL {
            WindowData* data = reinterpret_cast<WindowData*>(lParam);

            DWORD windowPid;
            GetWindowThreadProcessId(hwnd, &windowPid);

            if (windowPid == data->pid) {
                wchar_t title[256];
                GetWindowTextW(hwnd, title, sizeof(title) / sizeof(wchar_t));

                if (wcsstr(title, data->targetTitle.c_str()) != nullptr) {
                    data->found = true;
                    return FALSE;
                }
            }
            return TRUE;
            }, reinterpret_cast<LPARAM>(&data));

        return data.found;
    }

    DWORD WaitForProcess(const std::wstring& processName, DWORD timeoutMs) {
        auto startTime = std::chrono::steady_clock::now();

        while (std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now() - startTime).count() < timeoutMs) {

            DWORD pid = FindProcessId(processName);
            if (pid != 0) {
                return pid;
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        return 0;
    }

    bool IsProcessElevated(DWORD pid) {
        HANDLE hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
        if (!hProcess) {
            return false;
        }

        HANDLE hToken = NULL;
        TOKEN_ELEVATION elevation;
        DWORD dwSize;

        bool isElevated = false;

        if (OpenProcessToken(hProcess, TOKEN_QUERY, &hToken)) {
            if (GetTokenInformation(hToken, TokenElevation, &elevation, sizeof(elevation), &dwSize)) {
                isElevated = (elevation.TokenIsElevated != 0);
            }
            CloseHandle(hToken);
        }

        CloseHandle(hProcess);
        return isElevated;
    }

} // namespace ProcessUtils