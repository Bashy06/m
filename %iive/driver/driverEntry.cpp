#include <ntddk.h>
#include "defines.h"
#include "structure.h"
#include "ioctl.h"

// Forward declarations
DRIVER_UNLOAD DriverUnload;
DRIVER_DISPATCH IOCTLHandler;

// Global context
CR3_CONTEXT g_cr3_ctx = { 0 };

extern "C" NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath) {
    UNREFERENCED_PARAMETER(RegistryPath);

    KdPrint(("[RustCR3] Driver loading...\n"));

    // Setup driver functions
    DriverObject->DriverUnload = DriverUnload;
    DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = IOCTLHandler;

    // Initialize CR3 bypass
    NTSTATUS status = InitializeCR3Bypass(&g_cr3_ctx);
    if (!NT_SUCCESS(status)) {
        KdPrint(("[RustCR3] CR3 bypass init failed: 0x%X\n", status));
        return status;
    }

    // Create communication device
    UNICODE_STRING dev_name;
    RtlInitUnicodeString(&dev_name, DEVICE_NAME);

    PDEVICE_OBJECT device_obj;
    status = IoCreateDevice(DriverObject, 0, &dev_name, FILE_DEVICE_UNKNOWN, 0, FALSE, &device_obj);
    if (!NT_SUCCESS(status)) {
        CleanupCR3Bypass(&g_cr3_ctx);
        KdPrint(("[RustCR3] Device creation failed: 0x%X\n", status));
        return status;
    }

    // Create symbolic link
    UNICODE_STRING sym_name;
    RtlInitUnicodeString(&sym_name, SYM_LINK);
    status = IoCreateSymbolicLink(&sym_name, &dev_name);
    if (!NT_SUCCESS(status)) {
        IoDeleteDevice(device_obj);
        CleanupCR3Bypass(&g_cr3_ctx);
        KdPrint(("[RustCR3] Symbolic link failed: 0x%X\n", status));
        return status;
    }

    KdPrint(("[RustCR3] Driver loaded successfully!\n"));
    return STATUS_SUCCESS;
}

VOID DriverUnload(PDRIVER_OBJECT DriverObject) {
    UNICODE_STRING sym_name;
    RtlInitUnicodeString(&sym_name, SYM_LINK);
    IoDeleteSymbolicLink(&sym_name);

    if (DriverObject->DeviceObject) {
        IoDeleteDevice(DriverObject->DeviceObject);
    }

    CleanupCR3Bypass(&g_cr3_ctx);
    KdPrint(("[RustCR3] Driver unloaded.\n"));
}