#pragma once
#include <ntddk.h>

namespace CR3Bypass {
    ULONG_PTR GetProcessCr3(_In_ ULONG_PTR ProcessId);
    NTSTATUS ReadVirtualMemory(_In_ ULONG_PTR ProcessCr3, _In_ PVOID VirtualAddress, _Out_ PVOID Buffer, _In_ SIZE_T Size);
    NTSTATUS WriteVirtualMemory(_In_ ULONG_PTR ProcessCr3, _In_ PVOID VirtualAddress, _In_ PVOID Buffer, _In_ SIZE_T Size);
}