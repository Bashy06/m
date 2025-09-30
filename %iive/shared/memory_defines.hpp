#pragma once
#include <cstdint>

// IOCTL codes must match driver defines
#define IOCTL_READ_MEMORY 0x800
#define IOCTL_WRITE_MEMORY 0x801
#define IOCTL_GET_BASE 0x802

// Structures must match driver
typedef struct _MEMORY_READ_REQUEST {
    ULONG process_id;
    uintptr_t address;
    void* buffer;
    size_t size;
} MEMORY_READ_REQUEST;

typedef struct _MEMORY_WRITE_REQUEST {
    ULONG process_id;
    uintptr_t address;
    void* buffer;
    size_t size;
} MEMORY_WRITE_REQUEST;

typedef struct _BASE_ADDRESS_REQUEST {
    ULONG process_id;
    wchar_t module_name[256];
    uintptr_t base_address;
} BASE_ADDRESS_REQUEST;