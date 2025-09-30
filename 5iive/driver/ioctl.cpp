#include "ioctl.h"
#include "cr3_bypass.h"
#include "driver.h"

extern DRIVER_CONTEXT g_DriverContext;

NTSTATUS CreateDevice(_In_ PDRIVER_OBJECT DriverObject) {
    NTSTATUS status = STATUS_SUCCESS;

    // Initialize device name
    RtlInitUnicodeString(&g_DriverContext.DeviceName, DEVICE_NAME);

    // Create device
    status = IoCreateDevice(
        DriverObject,
        sizeof(DRIVER_CONTEXT),
        &g_DriverContext.DeviceName,
        FILE_DEVICE_UNKNOWN,
        FILE_DEVICE_SECURE_OPEN,
        FALSE,
        &g_DriverContext.DeviceObject
    );

    if (!NT_SUCCESS(status)) {
        DbgPrint("Failed to create device: 0x%X\n", status);
        return status;
    }

    // Create symbolic link
    RtlInitUnicodeString(&g_DriverContext.SymbolicLink, SYMBOLIC_LINK);
    status = IoCreateSymbolicLink(&g_DriverContext.SymbolicLink, &g_DriverContext.DeviceName);

    if (!NT_SUCCESS(status)) {
        DbgPrint("Failed to create symbolic link: 0x%X\n", status);
        IoDeleteDevice(g_DriverContext.DeviceObject);
        return status;
    }

    DbgPrint("Device created successfully: %wZ\n", &g_DriverContext.SymbolicLink);
    return STATUS_SUCCESS;
}

VOID DeleteDevice(_In_ PDRIVER_CONTEXT Context) {
    if (Context->SymbolicLink.Buffer) {
        IoDeleteSymbolicLink(&Context->SymbolicLink);
    }
    if (Context->DeviceObject) {
        IoDeleteDevice(Context->DeviceObject);
    }

    RtlZeroMemory(Context, sizeof(DRIVER_CONTEXT));
    DbgPrint("Device deleted successfully\n");
}