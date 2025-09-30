#include <Windows.h>
#include <iostream>
#include "..\driver\ioctl.h"

int main() {
    std::cout << "5iive Anti-Cheat Userland Loader" << std::endl;

    // Open driver handle
    HANDLE hDriver = CreateFileW(
        L"\\\\.\\FiveDriver",
        GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    if (hDriver == INVALID_HANDLE_VALUE) {
        std::cout << "Failed to open driver: " << GetLastError() << std::endl;
        return 1;
    }

    std::cout << "Driver opened successfully!" << std::endl;

    // Test communication with driver
    DWORD bytesReturned = 0;
    BOOL result = DeviceIoControl(
        hDriver,
        IOCTL_GET_CR3,
        NULL, 0,
        NULL, 0,
        &bytesReturned,
        NULL
    );

    if (result) {
        std::cout << "Driver communication successful!" << std::endl;
    }
    else {
        std::cout << "Driver communication failed: " << GetLastError() << std::endl;
    }

    CloseHandle(hDriver);
    std::cout << "Press any key to exit..." << std::endl;
    std::cin.get();
    return 0;
}