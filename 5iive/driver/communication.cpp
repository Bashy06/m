#include <ntddk.h>
#include "defines.h"
#include "structure.h"
#include "ioctl.h"

extern CR3_CONTEXT g_cr3_ctx;

NTSTATUS IOCTLHandler(PDEVICE_OBJECT DeviceObject, PIRP Irp) {
    UNREFERENCED_PARAMETER(DeviceObject);

    PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(Irp);
    ULONG control_code = stack->Parameters.DeviceIoControl.IoControlCode;
    PVOID input_buffer = Irp->AssociatedIrp.SystemBuffer;
    ULONG input_size = stack->Parameters.DeviceIoControl.InputBufferLength;

    NTSTATUS status = STATUS_SUCCESS;
    ULONG info = 0;

    switch (control_code) {
    case IOCTL_READ_MEMORY: {
        if (input_size == sizeof(MEMORY_READ_REQUEST)) {
            PMEMORY_READ_REQUEST request = (PMEMORY_READ_REQUEST)input_buffer;
            status = ReadPhysicalMemory(&g_cr3_ctx, request->process_id,
                (PVOID)request->address,
                request->buffer,
                request->size);
            info = request->size;
        }
        break;
    }

    case IOCTL_WRITE_MEMORY: {
        if (input_size == sizeof(MEMORY_WRITE_REQUEST)) {
            PMEMORY_WRITE_REQUEST request = (PMEMORY_WRITE_REQUEST)input_buffer;
            status = WritePhysicalMemory(&g_cr3_ctx, request->process_id,
                (PVOID)request->address,
                request->buffer,
                request->size);
        }
        break;
    }

    case IOCTL_GET_BASE: {
        if (input_size == sizeof(BASE_ADDRESS_REQUEST)) {
            PBASE_ADDRESS_REQUEST request = (PBASE_ADDRESS_REQUEST)input_buffer;
            request->base_address = GetProcessBase(request->process_id, request->module_name);
            info = sizeof(BASE_ADDRESS_REQUEST);
        }
        break;
    }

    default:
        status = STATUS_INVALID_DEVICE_REQUEST;
        break;
    }

    Irp->IoStatus.Status = status;
    Irp->IoStatus.Information = info;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);
    return status;
}