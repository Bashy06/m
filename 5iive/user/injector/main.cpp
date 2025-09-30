#include "ProcessUtils.h"
#include "MemoryReader.h"
#include "Overlay.h"
#include <iostream>
#include <thread>

class AntiCheatCore {
private:
    DWORD m_rustPid;
    bool m_running;

public:
    AntiCheatCore() : m_rustPid(0), m_running(false) {}

    bool Initialize() {
        std::cout << "Initializing Rust Anti-Cheat..." << std::endl;

        // Find Rust process
        m_rustPid = FindRustProcess();
        if (m_rustPid == 0) {
            std::cout << "Rust process not found. Waiting for it to start..." << std::endl;
            m_rustPid = ProcessUtils::WaitForProcess(L"RustClient.exe", 30000);
        }

        if (m_rustPid == 0) {
            std::cout << "Failed to find Rust process!" << std::endl;
            return false;
        }

        std::wcout << L"Found Rust process: " << m_rustPid << std::endl;

        // Initialize memory reader
        if (!MemoryReader::Initialize(m_rustPid)) {
            std::cout << "Failed to initialize memory reader!" << std::endl;
            return false;
        }

        // Initialize overlay
        if (!Overlay::Initialize()) {
            std::cout << "Failed to initialize overlay!" << std::endl;
            return false;
        }

        std::cout << "Anti-Cheat initialized successfully!" << std::endl;
        return true;
    }

    void Run() {
        m_running = true;
        std::cout << "Starting anti-cheat main loop..." << std::endl;

        // Main loop
        while (m_running) {
            // Check if Rust is still running
            if (!ProcessUtils::IsProcessRunning(m_rustPid)) {
                std::cout << "Rust process terminated. Exiting..." << std::endl;
                break;
            }

            // Update overlay
            Overlay::Render();

            // Perform memory checks
            PerformSecurityChecks();

            // Sleep to reduce CPU usage
            std::this_thread::sleep_for(std::chrono::milliseconds(16)); // ~60 FPS
        }

        Shutdown();
    }

    void Stop() {
        m_running = false;
    }

private:
    DWORD FindRustProcess() {
        // Try different possible Rust executable names
        const std::wstring rustNames[] = {
            L"RustClient.exe",
            L"rust.exe",
            L"rustclient.exe"
        };

        for (const auto& name : rustNames) {
            DWORD pid = ProcessUtils::FindProcessId(name);
            if (pid != 0) {
                std::wcout << L"Found " << name << L" with PID: " << pid << std::endl;
                return pid;
            }
        }
        return 0;
    }

    void PerformSecurityChecks() {
        // Example security checks
        CheckForSuspiciousModules();
        CheckMemoryIntegrity();
    }

    void CheckForSuspiciousModules() {
        // Implement module scanning logic
        // This is where you'd check for known cheat modules
    }

    void CheckMemoryIntegrity() {
        // Implement memory integrity checks
        // Check for code modifications, etc.
    }

    void Shutdown() {
        Overlay::Shutdown();
        MemoryReader::Shutdown();
        std::cout << "Anti-Cheat shutdown complete." << std::endl;
    }
};

int main() {
    AntiCheatCore anticheat;

    if (anticheat.Initialize()) {
        anticheat.Run();
    }
    else {
        std::cout << "Failed to initialize anti-cheat. Press any key to exit." << std::endl;
        std::cin.get();
        return -1;
    }

    return 0;
}