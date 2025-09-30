#include <windows.h>
#include <iostream>
#include "..\shared\memory_defines.hpp"

class KernelInterface {
private:
    HANDLE driver_handle;

public:
    bool connect() {
        driver_handle = CreateFileA("\\\\.\\RustCR3Driver",
            GENERIC_READ | GENERIC_WRITE,
            0, nullptr, OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL, nullptr);
        return driver_handle != INVALID_HANDLE_VALUE;
    }

    template<typename T>
    T read_memory(ULONG pid, uintptr_t address) {
        MEMORY_READ_REQUEST request;
        request.process_id = pid;
        request.address = address;
        request.size = sizeof(T);

        T buffer;
        request.buffer = &buffer;

        DWORD bytes_returned;
        DeviceIoControl(driver_handle, IOCTL_READ_MEMORY,
            &request, sizeof(request),
            &buffer, sizeof(T),
            &bytes_returned, nullptr);

        return buffer;
    }

    bool write_memory(ULONG pid, uintptr_t address, void* data, size_t size) {
        MEMORY_WRITE_REQUEST request;
        request.process_id = pid;
        request.address = address;
        request.buffer = data;
        request.size = size;

        DWORD bytes_returned;
        return DeviceIoControl(driver_handle, IOCTL_WRITE_MEMORY,
            &request, sizeof(request),
            nullptr, 0, &bytes_returned, nullptr);
    }

    uintptr_t get_module_base(ULONG pid, const wchar_t* module_name) {
        BASE_ADDRESS_REQUEST request;
        request.process_id = pid;
        wcscpy_s(request.module_name, module_name);

        DWORD bytes_returned;
        DeviceIoControl(driver_handle, IOCTL_GET_BASE,
            &request, sizeof(request),
            &request, sizeof(request),
            &bytes_returned, nullptr);

        return request.base_address;
    }
};