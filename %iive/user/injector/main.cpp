#include <windows.h>
#include <iostream>
#include "..\interface.cpp"

int main() {
    std::cout << "=== Rust CR3 Cheat Loader ===" << std::endl;

    KernelInterface kernel;
    if (kernel.connect()) {
        std::cout << "✅ Connected to kernel driver!" << std::endl;

        // Test with Rust process
        DWORD rust_pid = 1234; // You'll need to get actual PID
        uintptr_t base = kernel.get_module_base(rust_pid, L"GameAssembly.dll");

        if (base) {
            std::cout << "GameAssembly.dll base: 0x" << std::hex << base << std::dec << std::endl;
        }
    }
    else {
        std::cout << "❌ Failed to connect to driver" << std::endl;
        std::cout << "Make sure driver is loaded:" << std::endl;
        std::cout << "sc create RustCR3Driver binPath=C:\\path\\to\\driver.sys type=kernel" << std::endl;
        std::cout << "sc start RustCR3Driver" << std::endl;
    }

    std::cout << "Press Enter to exit..." << std::endl;
    std::cin.get();
    return 0;
}