#pragma once
#include "defines.h"
#include "structure.h"

// Function declarations
NTSTATUS InitializeCR3Bypass(PCR3_CONTEXT ctx);
VOID CleanupCR3Bypass(PCR3_CONTEXT ctx);
NTSTATUS ReadPhysicalMemory(PCR3_CONTEXT ctx, ULONG pid, PVOID address, PVOID buffer, SIZE_T size);
NTSTATUS WritePhysicalMemory(PCR3_CONTEXT ctx, ULONG pid, PVOID address, PVOID buffer, SIZE_T size);
ULONG_PTR GetProcessBase(ULONG pid, PCWSTR module_name);