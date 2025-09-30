#pragma once
#include <ntddk.h>
#include <wdf.h>

#define DRIVER_TAG '5ivE'

// Driver context
typedef struct _DRIVER_CONTEXT {
    PDEVICE_OBJECT DeviceObject;
    UNICODE_STRING DeviceName;
    UNICODE_STRING SymbolicLink;
} DRIVER_CONTEXT, *PDRIVER_CONTEXT;

// Driver entry prototype
extern "C" NTSTATUS DriverEntry(_In_ PDRIVER_OBJECT DriverObject, _In_ PUNICODE_STRING RegistryPath);