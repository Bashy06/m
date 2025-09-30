#include "driver.h"
#include "ioctl.h"

// Global context
DRIVER_CONTEXT g_DriverContext = { 0 };

extern "C" NTSTATUS DriverEntry(_In_ PDRIVER_OBJECT DriverObject, _In_ PUNICODE_STRING RegistryPath) {
    UNREFERENCED_PARAMETER(RegistryPath);

    NTSTATUS status = STATUS_SUCCESS;

    DbgPrint("5iive Driver Loading...\n");

    // Set up driver unload routine
    DriverObject->DriverUnload = [](_In_ PDRIVER_OBJECT DriverObject) {
        UNREFERENCED_PARAMETER(DriverObject);
        DbgPrint("5iive Driver Unloading...\n");
        DeleteDevice(&g_DriverContext);
        };

    // Set up dispatch routines
    for (int i = 0; i <= IRP_MJ_MAXIMUM_FUNCTION; i++) {
        DriverObject->MajorFunction[i] = [](_In_ PDEVICE_OBJECT DeviceObject, _In_ PIRP Irp) {
            UNREFERENCED_PARAMETER(DeviceObject);
            PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(Irp);

            switch (stack->MajorFunction) {
            case IRP_MJ_CREATE:
                DbgPrint("Handle opened to driver\n");
                break;
            case IRP_MJ_CLOSE:
                DbgPrint("Handle closed to driver\n");
                break;
            default:
                break;
            }

            Irp->IoStatus.Status = STATUS_SUCCESS;
            Irp->IoStatus.Information = 0;
            IoCompleteRequest(Irp, IO_NO_INCREMENT);
            return STATUS_SUCCESS;
            };
    }

    // Set specific dispatch routines
    DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = [](_In_ PDEVICE_OBJECT DeviceObject, _In_ PIRP Irp) {
        UNREFERENCED_PARAMETER(DeviceObject);
        // IOCTL handling will be implemented in ioctl.cpp
        Irp->IoStatus.Status = STATUS_SUCCESS;
        Irp->IoStatus.Information = 0;
        IoCompleteRequest(Irp, IO_NO_INCREMENT);
        return STATUS_SUCCESS;
        };

    // Create device
    status = CreateDevice(DriverObject);
    if (!NT_SUCCESS(status)) {
        DbgPrint("Failed to create device: 0x%X\n", status);
        return status;
    }

    DbgPrint("5iive Driver Loaded Successfully!\n");
    return STATUS_SUCCESS;
}