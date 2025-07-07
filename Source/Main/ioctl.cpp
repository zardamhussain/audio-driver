#include "definitions.h"
#include "mintopo.h"
#include "minwavert.h"
#include "minwavertstream.h"

#if MTCS_IOCTL_SUPPORT

// Global pointers to the miniport objects
// These will be set in AddDevice or StartDevice
// and used by the IOCTL handlers to access stream data.
extern class CMiniportWaveRTStream *   g_pCaptureMiniportStream;
extern class CMiniportWaveRTStream *   g_pRenderMiniportStream;


#pragma code_seg("PAGE")
NTSTATUS
CSMTIOCtlCreate
(
    _In_ DEVICE_OBJECT *DeviceObject,
    _Inout_ IRP *Irp
)
{
    UNREFERENCED_PARAMETER(DeviceObject);
    PAGED_CODE();
    
    // Add any necessary create handling here
    // For a backdoor, this might be minimal.

    Irp->IoStatus.Status = STATUS_SUCCESS;
    Irp->IoStatus.Information = 0;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);
    return STATUS_SUCCESS;
}

#pragma code_seg("PAGE")
NTSTATUS
CSMTIOCtlClose
(
    _In_ DEVICE_OBJECT *DeviceObject,
    _Inout_ IRP *Irp
)
{
    UNREFERENCED_PARAMETER(DeviceObject);
    PAGED_CODE();

    // Add any necessary close handling here

    Irp->IoStatus.Status = STATUS_SUCCESS;
    Irp->IoStatus.Information = 0;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);
    return STATUS_SUCCESS;
}

#pragma code_seg("PAGE")
NTSTATUS
CSMTIOCtlDeviceControl
(
    _In_ DEVICE_OBJECT *DeviceObject,
    _Inout_ IRP *Irp
)
{
    PAGED_CODE();

    NTSTATUS ntStatus = STATUS_NOT_SUPPORTED;
    PIO_STACK_LOCATION irpStack = IoGetCurrentIrpStackLocation(Irp);
    ULONG  ulIoControlCode = irpStack->Parameters.DeviceIoControl.IoControlCode;
    PVOID  pvInBuffer = Irp->AssociatedIrp.SystemBuffer;
    ULONG  ulInBufferLength = irpStack->Parameters.DeviceIoControl.InputBufferLength;
    PVOID  pvOutBuffer = Irp->AssociatedIrp.SystemBuffer; // For METHOD_BUFFERED, InBuffer and OutBuffer are the same
    ULONG  ulOutBufferLength = irpStack->Parameters.DeviceIoControl.OutputBufferLength;

    UNREFERENCED_PARAMETER(DeviceObject);

    switch (ulIoControlCode)
    {
        case IOCTL_MTCS_MIC_WRITE:
            // Handle writing data to the virtual microphone
            // This data will be fed to the audio engine as mic input
            if (g_pCaptureMiniportStream != NULL)
            {
                // Here, you would copy pvInBuffer data to the microphone stream's buffer.
                // This will likely involve casting g_pCaptureMiniportStream to your
                // CMiniportWaveRTStream class and calling a custom method to write data.
                // For now, let's just acknowledge the call.
                DPF(D_TERSE, ("IOCTL_MTCS_MIC_WRITE received, length: %d", ulInBufferLength));

                // *** ADDED FOR VERIFICATION ***
                // Print the received data if it's a string and within a reasonable size
                if (ulInBufferLength > 0 && ulInBufferLength < 256) 
                {
                    // Ensure null termination for safe printing
                    char buffer[256];
                    RtlZeroMemory(buffer, sizeof(buffer));
                    RtlCopyMemory(buffer, pvInBuffer, MIN(ulInBufferLength, sizeof(buffer) - 1));
                    DPF(D_TERSE, ("Received Mic Data: %s", buffer));
                }
                // *** END ADDED FOR VERIFICATION ***

                // Write data to the capture stream (virtual microphone)
                g_pCaptureMiniportStream->WriteBytes(pvInBuffer, ulInBufferLength);
                ntStatus = STATUS_SUCCESS;
                Irp->IoStatus.Information = ulInBufferLength; // Indicate bytes written
            }
            else
            {
                DPF(D_ERROR, ("IOCTL_MTCS_MIC_WRITE: Capture stream not initialized"));
                ntStatus = STATUS_INVALID_DEVICE_STATE;
            }
            break;

        case IOCTL_MTCS_SPEAKER_READ:
            // Handle reading data from the virtual speaker
            // This data would be what the audio engine is playing through the virtual speaker
            if (g_pRenderMiniportStream != NULL)
            {
                // Here, you would copy data from the speaker stream's buffer to pvOutBuffer.
                // This will likely involve casting g_pRenderMiniportStream to your
                // CMiniportWaveRTStream class and calling a custom method to read data.
                // For now, let's just acknowledge the call.
                DPF(D_TERSE, ("IOCTL_MTCS_SPEAKER_READ received, buffer length: %d", ulOutBufferLength));
                // Read data from the render stream (virtual speaker)
                g_pRenderMiniportStream->ReadBytes(pvOutBuffer, ulOutBufferLength);
                ntStatus = STATUS_SUCCESS;
                Irp->IoStatus.Information = ulOutBufferLength; // Indicate bytes read
            }
            else
            {
                DPF(D_ERROR, ("IOCTL_MTCS_SPEAKER_READ: Render stream not initialized"));
                ntStatus = STATUS_INVALID_DEVICE_STATE;
            }
            break;

        default:
            // If it's not our custom IOCTL, pass it to the original handler (Port Class)
            if (g_pfnOriginalDeviceControl)
            {
                ntStatus = g_pfnOriginalDeviceControl(DeviceObject, Irp);
            }
            else
            {
                DPF(D_ERROR, ("Unknown IOCTL and no original dispatch routine: 0x%x", ulIoControlCode));
                ntStatus = STATUS_INVALID_PARAMETER;
            }
            break;
    }

    Irp->IoStatus.Status = ntStatus;
    // For custom IOCTLs, IoCompleteRequest is called here.
    // For calls passed to g_pfnOriginalDeviceControl, that routine will complete the IRP.
    if (ulIoControlCode == IOCTL_MTCS_MIC_WRITE || ulIoControlCode == IOCTL_MTCS_SPEAKER_READ)
    {
        IoCompleteRequest(Irp, IO_NO_INCREMENT);
    }
    
    return ntStatus;
}

#endif // MTCS_IOCTL_SUPPORT 