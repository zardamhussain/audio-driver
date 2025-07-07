#include <iostream>
#include <string>
#include <windows.h>
#include <setupapi.h>
#include <devguid.h>
#include <initguid.h>
#include <winioctl.h>

// Define the same GUID and IOCTLs as in the driver
DEFINE_GUID(GUID_DEVINTERFACE_MTCS_AUDIO_BACKDOOR, 
0x05513b4d, 0x6462, 0x45e1, 0x8c, 0xbd, 0x0a, 0x43, 0xac, 0x17, 0x6b, 0x91);

#define IOCTL_MTCS_MIC_WRITE CTL_CODE(FILE_DEVICE_UNKNOWN, 0x900, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_MTCS_SPEAKER_READ CTL_CODE(FILE_DEVICE_UNKNOWN, 0x901, METHOD_BUFFERED, FILE_ANY_ACCESS)

int main()
{
    HDEVINFO hDevInfo;
    SP_DEVICE_INTERFACE_DATA DeviceInterfaceData;
    PSP_DEVICE_INTERFACE_DETAIL_DATA pDeviceInterfaceDetailData = NULL;
    ULONG ulLen;
    HANDLE hDevice = INVALID_HANDLE_VALUE;
    BOOL bResult;

    std::wcout << L"Attempting to find device..." << std::endl;

    // Get a handle to the device information set for our device interface GUID
    hDevInfo = SetupDiGetClassDevs(
        &GUID_DEVINTERFACE_MTCS_AUDIO_BACKDOOR, 
        NULL, 
        NULL, 
        DIGCF_PRESENT | DIGCF_DEVICEINTERFACE
    );

    if (hDevInfo == INVALID_HANDLE_VALUE)
    {
        std::wcerr << L"SetupDiGetClassDevs failed: " << GetLastError() << std::endl;
        return 1;
    }

    DeviceInterfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);

    // Enumerate device interfaces
    if (!SetupDiEnumDeviceInterfaces(hDevInfo, NULL, &GUID_DEVINTERFACE_MTCS_AUDIO_BACKDOOR, 0, &DeviceInterfaceData))
    {
        std::wcerr << L"SetupDiEnumDeviceInterfaces failed: " << GetLastError() << std::endl;
        SetupDiDestroyDeviceInfoList(hDevInfo);
        return 1;
    }

    // Get the required buffer size
    SetupDiGetDeviceInterfaceDetail(
        hDevInfo,
        &DeviceInterfaceData,
        NULL,
        0,
        &ulLen,
        NULL
    );

    pDeviceInterfaceDetailData = (PSP_DEVICE_INTERFACE_DETAIL_DATA)LocalAlloc(LMEM_FIXED, ulLen);
    if (!pDeviceInterfaceDetailData)
    {
        std::wcerr << L"LocalAlloc failed: " << GetLastError() << std::endl;
        SetupDiDestroyDeviceInfoList(hDevInfo);
        return 1;
    }
    pDeviceInterfaceDetailData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);

    // Get the device path
    if (!SetupDiGetDeviceInterfaceDetail(
        hDevInfo,
        &DeviceInterfaceData,
        pDeviceInterfaceDetailData,
        ulLen,
        NULL,
        NULL
    ))
    {
        std::wcerr << L"SetupDiGetDeviceInterfaceDetail failed: " << GetLastError() << std::endl;
        LocalFree(pDeviceInterfaceDetailData);
        SetupDiDestroyDeviceInfoList(hDevInfo);
        return 1;
    }

    std::wcout << L"Device Path: " << pDeviceInterfaceDetailData->DevicePath << std::endl;

    // Open a handle to the device
    hDevice = CreateFile(
        pDeviceInterfaceDetailData->DevicePath,
        GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, // Use FILE_FLAG_OVERLAPPED for asynchronous operations
        NULL
    );

    if (hDevice == INVALID_HANDLE_VALUE)
    {
        std::wcerr << L"CreateFile failed: " << GetLastError() << std::endl;
        LocalFree(pDeviceInterfaceDetailData);
        SetupDiDestroyDeviceInfoList(hDevInfo);
        return 1;
    }

    std::wcout << L"Successfully opened device handle." << std::endl;

    // Example: Send data to the virtual microphone
    const char micData[] = "Hello from user-mode application!";
    DWORD bytesWritten;
    bResult = DeviceIoControl(
        hDevice,
        IOCTL_MTCS_MIC_WRITE,
        (LPVOID)micData,
        sizeof(micData),
        NULL,
        0,
        &bytesWritten,
        NULL
    );

    if (!bResult)
    {
        std::wcerr << L"IOCTL_MTCS_MIC_WRITE failed: " << GetLastError() << std::endl;
    }
    else
    {
        std::wcout << L"IOCTL_MTCS_MIC_WRITE successful, bytes written: " << bytesWritten << std::endl;
    }

    // Example: Read data from the virtual speaker
    char speakerData[256];
    DWORD bytesRead;
    bResult = DeviceIoControl(
        hDevice,
        IOCTL_MTCS_SPEAKER_READ,
        NULL,
        0,
        (LPVOID)speakerData,
        sizeof(speakerData),
        &bytesRead,
        NULL
    );

    if (!bResult)
    {
        std::wcerr << L"IOCTL_MTCS_SPEAKER_READ failed: " << GetLastError() << std::endl;
    }
    else
    {
        std::wcout << L"IOCTL_MTCS_SPEAKER_READ successful, bytes read: " << bytesRead << std::endl;
        // For demonstration, print the first few bytes (assuming it's readable text)
        // In a real scenario, this would be audio data.
        // std::cout.write(speakerData, bytesRead < sizeof(speakerData) ? bytesRead : sizeof(speakerData));
        // std::cout << std::endl;
    }

    // Clean up
    CloseHandle(hDevice);
    LocalFree(pDeviceInterfaceDetailData);
    SetupDiDestroyDeviceInfoList(hDevInfo);

    std::wcout << L"Application finished." << std::endl;

    return 0;
} 