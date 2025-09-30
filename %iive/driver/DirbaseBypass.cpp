#include <ntddk.h>
#include "defines.h"
#include "structure.h"
#include "ioctl.h"

NTSTATUS InitializeCR3Bypass(PCR3_CONTEXT ctx) {
    KdPrint(("[RustCR3] Initializing CR3 bypass...\n"));

    if (!ctx) return STATUS_INVALID_PARAMETER;

    ctx->original_cr3 = __readcr3();
    ctx->is_initialized = TRUE;

    KdPrint(("[RustCR3] CR3 bypass ready. CR3: 0x%llX\n", ctx->original_cr3));
    return STATUS_SUCCESS;
}

VOID CleanupCR3Bypass(PCR3_CONTEXT ctx) {
    if (ctx && ctx->is_initialized) {
        ctx->is_initialized = FALSE;
        KdPrint(("[RustCR3] CR3 bypass cleaned\n"));
    }
}

NTSTATUS ReadPhysicalMemory(PCR3_CONTEXT ctx, ULONG pid, PVOID address, PVOID buffer, SIZE_T size) {
    if (!ctx || !ctx->is_initialized) return STATUS_UNSUCCESSFUL;

    PEPROCESS target_process;
    NTSTATUS status = PsLookupProcessByProcessId((HANDLE)pid, &target_process);
    if (!NT_SUCCESS(status)) return status;

    KAPC_STATE apc_state;
    KeStackAttachProcess(target_process, &apc_state);

    __try {
        RtlCopyMemory(buffer, address, size);
        status = STATUS_SUCCESS;
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
        status = GetExceptionCode();
    }

    KeUnstackDetachProcess(&apc_state);
    ObDereferenceObject(target_process);
    return status;
}

NTSTATUS WritePhysicalMemory(PCR3_CONTEXT ctx, ULONG pid, PVOID address, PVOID buffer, SIZE_T size) {
    if (!ctx || !ctx->is_initialized) return STATUS_UNSUCCESSFUL;

    PEPROCESS target_process;
    NTSTATUS status = PsLookupProcessByProcessId((HANDLE)pid, &target_process);
    if (!NT_SUCCESS(status)) return status;

    KAPC_STATE apc_state;
    KeStackAttachProcess(target_process, &apc_state);

    __try {
        RtlCopyMemory(address, buffer, size);
        status = STATUS_SUCCESS;
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
        status = GetExceptionCode();
    }

    KeUnstackDetachProcess(&apc_state);
    ObDereferenceObject(target_process);
    return status;
}

ULONG_PTR GetProcessBase(ULONG pid, PCWSTR module_name) {
    PEPROCESS target_process;
    NTSTATUS status = PsLookupProcessByProcessId((HANDLE)pid, &target_process);
    if (!NT_SUCCESS(status)) return 0;

    ULONG_PTR base_address = 0;
    KAPC_STATE apc_state;
    KeStackAttachProcess(target_process, &apc_state);

    __try {
        PPEB peb = PsGetProcessPeb(target_process);
        if (peb && peb->Ldr) {
            for (PLIST_ENTRY entry = peb->Ldr->InMemoryOrderModuleList.Flink;
                entry != &peb->Ldr->InMemoryOrderModuleList;
                entry = entry->Flink) {

                PLDR_DATA_TABLE_ENTRY module = CONTAINING_RECORD(entry, LDR_DATA_TABLE_ENTRY, InMemoryOrderLinks);

                if (module->BaseDllName.Buffer) {
                    UNICODE_STRING target_name;
                    RtlInitUnicodeString(&target_name, module_name);

                    if (RtlCompareUnicodeString(&module->BaseDllName, &target_name, TRUE) == 0) {
                        base_address = (ULONG_PTR)module->DllBase;
                        break;
                    }
                }
            }
        }
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
        base_address = 0;
    }

    KeUnstackDetachProcess(&apc_state);
    ObDereferenceObject(target_process);
    return base_address;
}