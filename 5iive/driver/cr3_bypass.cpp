#include "cr3_bypass.h"
#include <ntddk.h>

namespace CR3Bypass {

    ULONG_PTR GetProcessCr3(_In_ ULONG_PTR ProcessId) {
        PEPROCESS Process = nullptr;
        NTSTATUS status = PsLookupProcessByProcessId((HANDLE)ProcessId, &Process);

        if (!NT_SUCCESS(status)) {
            return 0;
        }

        // Get DirectoryTableBase (CR3) from EPROCESS
        ULONG_PTR cr3 = *(ULONG_PTR*)((PUCHAR)Process + 0x28); // _KPROCESS.DirectoryTableBase

        ObDereferenceObject(Process);
        return cr3;
    }

    NTSTATUS ReadVirtualMemory(_In_ ULONG_PTR ProcessCr3, _In_ PVOID VirtualAddress, _Out_ PVOID Buffer, _In_ SIZE_T Size) {
        if (!Buffer || !VirtualAddress || Size == 0) {
            return STATUS_INVALID_PARAMETER;
        }

        // Save current CR3
        ULONG_PTR originalCr3 = __readcr3();

        __try {
            // Switch to target process CR3
            __writecr3(ProcessCr3);

            // Perform the memory copy
            RtlCopyMemory(Buffer, VirtualAddress, Size);

            // Restore original CR3
            __writecr3(originalCr3);

            return STATUS_SUCCESS;
        }
        __except (EXCEPTION_EXECUTE_HANDLER) {
            // Restore CR3 on exception
            __writecr3(originalCr3);
            return STATUS_ACCESS_VIOLATION;
        }
    }

    NTSTATUS WriteVirtualMemory(_In_ ULONG_PTR ProcessCr3, _In_ PVOID VirtualAddress, _In_ PVOID Buffer, _In_ SIZE_T Size) {
        if (!Buffer || !VirtualAddress || Size == 0) {
            return STATUS_INVALID_PARAMETER;
        }

        // Save current CR3
        ULONG_PTR originalCr3 = __readcr3();

        __try {
            // Switch to target process CR3
            __writecr3(ProcessCr3);

            // Perform the memory copy
            RtlCopyMemory(VirtualAddress, Buffer, Size);

            // Restore original CR3
            __writecr3(originalCr3);

            return STATUS_SUCCESS;
        }
        __except (EXCEPTION_EXECUTE_HANDLER) {
            // Restore CR3 on exception
            __writecr3(originalCr3);
            return STATUS_ACCESS_VIOLATION;
        }
    }
}