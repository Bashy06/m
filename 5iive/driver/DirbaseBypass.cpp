#include "DirbaseBypass.h"
#include <ntddk.h>

namespace CR3Bypass {
    // Get process CR3 from EPROCESS
    ULONG_PTR GetProcessCr3(PEPROCESS Process) {
        return *(ULONG_PTR*)((PUCHAR)Process + 0x28); // _KPROCESS.DirectoryTableBase
    }

    // Read physical memory using CR3
    NTSTATUS ReadPhysicalMemory(ULONG_PTR TargetCr3, PVOID VirtualAddress, PVOID Buffer, SIZE_T Size) {
        PMDL Mdl = NULL;
        PVOID MappedAddress = NULL;
        NTSTATUS Status = STATUS_SUCCESS;

        // Create MDL for the virtual address
        Mdl = IoAllocateMdl(VirtualAddress, (ULONG)Size, FALSE, FALSE, NULL);
        if (!Mdl) {
            return STATUS_INSUFFICIENT_RESOURCES;
        }

        __try {
            // Probe and lock pages
            MmProbeAndLockPages(Mdl, KernelMode, IoReadAccess);

            // Map pages to system address space
            MappedAddress = MmMapLockedPagesSpecifyCache(Mdl, KernelMode, MmNonCached, NULL, FALSE, NormalPagePriority);
            if (!MappedAddress) {
                MmUnlockPages(Mdl);
                IoFreeMdl(Mdl);
                return STATUS_UNSUCCESSFUL;
            }

            // Switch CR3 and perform copy
            ULONG_PTR OriginalCr3 = __readcr3();
            __writecr3(TargetCr3);

            RtlCopyMemory(Buffer, MappedAddress, Size);

            // Restore original CR3
            __writecr3(OriginalCr3);

            MmUnmapLockedPages(MappedAddress, Mdl);
            MmUnlockPages(Mdl);
        }
        __except (EXCEPTION_EXECUTE_HANDLER) {
            Status = STATUS_ACCESS_VIOLATION;
        }

        IoFreeMdl(Mdl);
        return Status;
    }

    // Main memory read function
    NTSTATUS ReadVirtualMemory(ULONG_PTR ProcessCr3, PVOID Address, PVOID Buffer, SIZE_T Size) {
        return ReadPhysicalMemory(ProcessCr3, Address, Buffer, Size);
    }
}