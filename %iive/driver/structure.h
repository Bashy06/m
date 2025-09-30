#pragma once

typedef struct _CR3_CONTEXT {
    ULONG_PTR original_cr3;
    BOOLEAN is_initialized;
} CR3_CONTEXT, * PCR3_CONTEXT;

typedef struct _MEMORY_READ_REQUEST {
    ULONG process_id;
    ULONG_PTR address;
    PVOID buffer;
    SIZE_T size;
} MEMORY_READ_REQUEST, * PMEMORY_READ_REQUEST;

typedef struct _MEMORY_WRITE_REQUEST {
    ULONG process_id;
    ULONG_PTR address;
    PVOID buffer;
    SIZE_T size;
} MEMORY_WRITE_REQUEST, * PMEMORY_WRITE_REQUEST;

typedef struct _BASE_ADDRESS_REQUEST {
    ULONG process_id;
    WCHAR module_name[256];
    ULONG_PTR base_address;
} BASE_ADDRESS_REQUEST, * PBASE_ADDRESS_REQUEST;// Shared Kernel Structures
