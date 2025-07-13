/*++

Copyright (c) 1998  Intel Corporation

Module Name:

    data.c

Abstract:

    EFI library global data



Revision History

--*/


#include "efibind.h"
#include "efierr.h"
#include "lib.h"


//
// LibInitialized - TRUE once InitializeLib() is called for the first time
//

BOOLEAN  LibInitialized = FALSE;

//
// ImageHandle - Current ImageHandle, as passed to InitializeLib
//
EFI_HANDLE LibImageHandle;

//
// ST - pointer to the EFI system table
//

EFI_SYSTEM_TABLE        *ST;

//
// BS - pointer to the boot services table
//

EFI_BOOT_SERVICES       *BS;


//
// Default pool allocation type
//

EFI_MEMORY_TYPE PoolAllocationType = EfiBootServicesData;

//
// Unicode collation functions that are in use
//

EFI_UNICODE_COLLATION_INTERFACE   LibStubUnicodeInterface = {
    (EFI_UNICODE_STRICOLL)NULL,
    (EFI_UNICODE_METAIMATCH)NULL,
    (EFI_UNICODE_STRLWR)NULL,
    (EFI_UNICODE_STRUPR)NULL,
    (EFI_UNICODE_FATTOSTR)NULL,   // FatToStr
    (EFI_UNICODE_STRTOFAT)NULL,   // StrToFat
    (CHAR8*)NULL                  // SupportedLanguages
};

EFI_UNICODE_COLLATION_INTERFACE   *UnicodeInterface = &LibStubUnicodeInterface;

//
// Root device path
//

EFI_DEVICE_PATH RootDevicePath[] = {
   {END_DEVICE_PATH_TYPE, END_ENTIRE_DEVICE_PATH_SUBTYPE, {END_DEVICE_PATH_LENGTH,0}}
};

EFI_DEVICE_PATH EndDevicePath[] = {
   {END_DEVICE_PATH_TYPE, END_ENTIRE_DEVICE_PATH_SUBTYPE, {END_DEVICE_PATH_LENGTH, 0}}
};

EFI_DEVICE_PATH EndInstanceDevicePath[] = {
   {END_DEVICE_PATH_TYPE, END_INSTANCE_DEVICE_PATH_SUBTYPE, {END_DEVICE_PATH_LENGTH, 0}}
};


//
// EFI IDs
//
EFI_GUID gEfiGlobalVariableGuid = EFI_GLOBAL_VARIABLE;
EFI_GUID NullGuid = { 0,0,0,{0,0,0,0,0,0,0,0} };

//
// Protocol IDs
//
EFI_GUID gEfiDevicePathProtocolGuid                 = EFI_DEVICE_PATH_PROTOCOL_GUID;
EFI_GUID gEfiDevicePathToTextProtocolGuid           = EFI_DEVICE_PATH_TO_TEXT_PROTOCOL_GUID;
EFI_GUID gEfiDevicePathFromTextProtocolGuid         = EFI_DEVICE_PATH_FROM_TEXT_PROTOCOL_GUID;
EFI_GUID gEfiDevicePathUtilitiesProtocolGuid        = EFI_DEVICE_PATH_UTILITIES_PROTOCOL_GUID;
EFI_GUID gEfiLoadedImageProtocolGuid                = EFI_LOADED_IMAGE_PROTOCOL_GUID;
EFI_GUID gEfiSimpleTextInProtocolGuid               = EFI_SIMPLE_TEXT_INPUT_PROTOCOL_GUID;
EFI_GUID gEfiSimpleTextOutProtocolGuid              = EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL_GUID;
EFI_GUID gEfiBlockIoProtocolGuid                    = EFI_BLOCK_IO_PROTOCOL_GUID;
EFI_GUID gEfiBlockIo2ProtocolGuid                   = EFI_BLOCK_IO2_PROTOCOL_GUID;
EFI_GUID gEfiDiskIoProtocolGuid                     = EFI_DISK_IO_PROTOCOL_GUID;
EFI_GUID gEfiDiskIo2ProtocolGuid                    = EFI_DISK_IO2_PROTOCOL_GUID;
EFI_GUID gEfiSimpleFileSystemProtocolGuid           = EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID;
EFI_GUID gEfiLoadFileProtocolGuid                   = EFI_LOAD_FILE_PROTOCOL_GUID;
EFI_GUID gEfiDeviceIoProtocolGuid                   = EFI_DEVICE_IO_PROTOCOL_GUID;
EFI_GUID gEfiUnicodeCollationProtocolGuid           = EFI_UNICODE_COLLATION_PROTOCOL_GUID;
EFI_GUID gEfiSerialIoProtocolGuid                   = EFI_SERIAL_IO_PROTOCOL_GUID;
EFI_GUID gEfiSimpleNetworkProtocolGuid              = EFI_SIMPLE_NETWORK_PROTOCOL_GUID;
EFI_GUID gEfiPxeBaseCodeProtocolGuid                = EFI_PXE_BASE_CODE_PROTOCOL_GUID;
EFI_GUID gEfiPxeBaseCodeCallbackProtocolGuid        = EFI_PXE_BASE_CODE_CALLBACK_PROTOCOL_GUID;
EFI_GUID gEfiNetworkInterfaceIdentifierProtocolGuid = EFI_NETWORK_INTERFACE_IDENTIFIER_PROTOCOL_GUID;
EFI_GUID gEFiUiInterfaceProtocolGuid                = EFI_UI_INTERFACE_PROTOCOL_GUID;
EFI_GUID gEfiPciIoProtocolGuid                      = EFI_PCI_IO_PROTOCOL_GUID;
EFI_GUID gEfiPciRootBridgeIoProtocolGuid            = EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_GUID;
EFI_GUID gEfiDriverBindingProtocolGuid              = EFI_DRIVER_BINDING_PROTOCOL_GUID;
EFI_GUID gEfiComponentNameProtocolGuid              = EFI_COMPONENT_NAME_PROTOCOL_GUID;
EFI_GUID gEfiComponentName2ProtocolGuid             = EFI_COMPONENT_NAME2_PROTOCOL_GUID;
EFI_GUID gEfiHashProtocolGuid                       = EFI_HASH_PROTOCOL_GUID;
EFI_GUID gEfiPlatformDriverOverrideProtocolGuid     = EFI_PLATFORM_DRIVER_OVERRIDE_PROTOCOL_GUID;
EFI_GUID gEfiBusSpecificDriverOverrideProtocolGuid  = EFI_BUS_SPECIFIC_DRIVER_OVERRIDE_PROTOCOL_GUID;
EFI_GUID gEfiDriverFamilyOverrideProtocolGuid       = EFI_DRIVER_FAMILY_OVERRIDE_PROTOCOL_GUID;
EFI_GUID gEfiEbcProtocolGuid                        = EFI_EBC_PROTOCOL_GUID;

//
// File system information IDs
//

EFI_GUID gEfiFileInfoGuid                           = EFI_FILE_INFO_ID;
EFI_GUID gEfiFileSystemInfoGuid                     = EFI_FILE_SYSTEM_INFO_ID;
EFI_GUID gEfiFileSystemVolumeLabelInfoIdGuid        = EFI_FILE_SYSTEM_VOLUME_LABEL_ID;

//
// Reference implementation public protocol IDs
//

EFI_GUID InternalShellProtocol = INTERNAL_SHELL_GUID;
EFI_GUID VariableStoreProtocol = VARIABLE_STORE_PROTOCOL;
EFI_GUID LegacyBootProtocol = LEGACY_BOOT_PROTOCOL;
EFI_GUID VgaClassProtocol = VGA_CLASS_DRIVER_PROTOCOL;

EFI_GUID TextOutSpliterProtocol = TEXT_OUT_SPLITER_PROTOCOL;
EFI_GUID ErrorOutSpliterProtocol = ERROR_OUT_SPLITER_PROTOCOL;
EFI_GUID TextInSpliterProtocol = TEXT_IN_SPLITER_PROTOCOL;
/* Added for GOP support */
EFI_GUID gEfiGraphicsOutputProtocolGuid             = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
EFI_GUID gEfiEdidDiscoveredProtocolGuid             = EFI_EDID_DISCOVERED_PROTOCOL_GUID;
EFI_GUID gEfiEdidActiveProtocolGuid                 = EFI_EDID_ACTIVE_PROTOCOL_GUID;
EFI_GUID gEfiEdidOverrideProtocolGuid               = EFI_EDID_OVERRIDE_PROTOCOL_GUID;

EFI_GUID AdapterDebugProtocol = ADAPTER_DEBUG_PROTOCOL;

//
// Device path media protocol IDs
//
EFI_GUID gEfiPcAnsiGuid                             = EFI_PC_ANSI_GUID;
EFI_GUID gEfiVT100Guid                              = EFI_VT_100_GUID;
EFI_GUID gEfiVT100PlusGuid                          = EFI_VT_100_PLUS_GUID;
EFI_GUID gEfiVTUTF8Guid                             = EFI_VT_UTF8_GUID;

//
// EFI GPT Partition Type GUIDs
//
EFI_GUID EfiPartTypeSystemPartitionGuid = EFI_PART_TYPE_EFI_SYSTEM_PART_GUID;
EFI_GUID EfiPartTypeLegacyMbrGuid = EFI_PART_TYPE_LEGACY_MBR_GUID;


//
// Reference implementation Vendor Device Path Guids
//
EFI_GUID UnknownDevice      = UNKNOWN_DEVICE_GUID;

//
// Configuration Table GUIDs
//

EFI_GUID MpsTableGuid             = MPS_TABLE_GUID;
EFI_GUID AcpiTableGuid            = ACPI_TABLE_GUID;
EFI_GUID SMBIOSTableGuid          = SMBIOS_TABLE_GUID;
EFI_GUID SMBIOS3TableGuid         = SMBIOS3_TABLE_GUID;
EFI_GUID SalSystemTableGuid       = SAL_SYSTEM_TABLE_GUID;
EFI_GUID EfiDtbTableGuid          = EFI_DTB_TABLE_GUID;

//
// Network protocol GUIDs
//
EFI_GUID Ip4ServiceBindingProtocol = EFI_IP4_SERVICE_BINDING_PROTOCOL;
EFI_GUID Ip4Protocol               = EFI_IP4_PROTOCOL;
EFI_GUID Udp4ServiceBindingProtocol = EFI_UDP4_SERVICE_BINDING_PROTOCOL;
EFI_GUID Udp4Protocol              = EFI_UDP4_PROTOCOL;
EFI_GUID Tcp4ServiceBindingProtocol = EFI_TCP4_SERVICE_BINDING_PROTOCOL;
EFI_GUID Tcp4Protocol              = EFI_TCP4_PROTOCOL;

//
// Pointer protocol GUIDs
//
EFI_GUID SimplePointerProtocol   = EFI_SIMPLE_POINTER_PROTOCOL_GUID;
EFI_GUID AbsolutePointerProtocol = EFI_ABSOLUTE_POINTER_PROTOCOL_GUID;

//
// Debugger protocol GUIDs
//
EFI_GUID gEfiDebugImageInfoTableGuid  = EFI_DEBUG_IMAGE_INFO_TABLE_GUID;
EFI_GUID gEfiDebugSupportProtocolGuid = EFI_DEBUG_SUPPORT_PROTOCOL_GUID;

//
// Console extension protocol GUIDs
//
EFI_GUID SimpleTextInputExProtocol = EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL_GUID;

//
// Shell protocol GUIDs
//
EFI_GUID ShellProtocolGuid               = EFI_SHELL_PROTOCOL_GUID;
EFI_GUID ShellParametersProtocolGuid     = EFI_SHELL_PARAMETERS_PROTOCOL_GUID;
EFI_GUID ShellDynamicCommandProtocolGuid = EFI_SHELL_DYNAMIC_COMMAND_PROTOCOL_GUID;


INTN CompareGuid (IN EFI_GUID *Guid1, IN EFI_GUID *Guid2)
{
    INT32* g1;
    INT32* g2;
    INT32  r;

    //
    // Compare 32 bits at a time
    //

    g1 = (INT32 *) Guid1;
    g2 = (INT32 *) Guid2;

    r  = g1[0] - g2[0];
    r |= g1[1] - g2[1];
    r |= g1[2] - g2[2];
    r |= g1[3] - g2[3];

    return r;
}


EFI_STATUS LibGetSystemConfigurationTable(
    IN EFI_GUID *TableGuid,
    IN OUT VOID **Table
) {
    UINTN Index;

    for(Index=0;Index<ST->NumberOfTableEntries;Index++) {
        if (CompareGuid(TableGuid, &(ST->ConfigurationTable[Index].VendorGuid)) == 0) {
            *Table = ST->ConfigurationTable[Index].VendorTable;
            return EFI_SUCCESS;
        }
    }
    return EFI_NOT_FOUND;
}


VOID
EFIDebugVariable (
    VOID
    )
{
    EFI_STATUS      Status;
    UINT32          Attributes;
    UINTN           DataSize;
    UINTN           NewEFIDebug;

    DataSize = sizeof(EFIDebug);
    Status = gRT->GetVariable((CHAR16*)L"EFIDebug", &EfiGlobalVariable, &Attributes, &DataSize, &NewEFIDebug);
    if (!EFI_ERROR(Status)) {
        EFIDebug = NewEFIDebug;
    }
    return;
}



BOOLEAN
GrowBuffer(
    IN OUT EFI_STATUS   *Status,
    IN OUT VOID         **Buffer,
    IN UINTN            BufferSize
) {
    BOOLEAN         TryAgain;

    //
    // If this is an initial request, buffer will be null with a new buffer size
    //

    if (!*Buffer && BufferSize) {
        *Status = EFI_BUFFER_TOO_SMALL;
    }

    //
    // If the status code is "buffer too small", resize the buffer
    //
        
    TryAgain = FALSE;
    if (*Status == EFI_BUFFER_TOO_SMALL) {

        if (*Buffer) {
            uefi_call_wrapper(gBS->FreePool, 1, *Buffer);
        }


        *Status = uefi_call_wrapper(gBS->AllocatePool, 3, EfiBootServicesData, BufferSize, Buffer);
        if (*Buffer) {
            TryAgain = TRUE;
        } else {    
            *Status = EFI_OUT_OF_RESOURCES;
        } 
    }

    //
    // If there's an error, free the buffer
    //

    if (!TryAgain && EFI_ERROR(*Status) && *Buffer) {
        uefi_call_wrapper(gBS->FreePool, 1, *Buffer);
        *Buffer = NULL;
    }

    return TryAgain;
}


VOID *
LibGetVariableAndSize (
    IN CHAR16               *Name,
    IN EFI_GUID             *VendorGuid,
    OUT UINTN               *VarSize
    )
{
    EFI_STATUS              Status = EFI_SUCCESS;
    VOID                    *Buffer;
    UINTN                   BufferSize;


    Buffer = NULL;
    BufferSize = 100;

    while (GrowBuffer (&Status, &Buffer, BufferSize)) {
        Status = gRT->GetVariable(
            Name,
            VendorGuid,
            (UINT32*)NULL,
            &BufferSize,
            Buffer
        );
    }
    if (Buffer) {
        *VarSize = BufferSize;
    } else {
        *VarSize = 0;
    }
    return Buffer;
}
    
VOID *
LibGetVariable (
    IN CHAR16               *Name,
    IN EFI_GUID             *VendorGuid
    )
{
    UINTN   VarSize;

    return LibGetVariableAndSize (Name, VendorGuid, &VarSize);
}


VOID
InitializeLib (
    IN EFI_HANDLE           ImageHandle,
    IN EFI_SYSTEM_TABLE     *SystemTable
) {
    EFI_LOADED_IMAGE        *LoadedImage;
    EFI_STATUS              Status = EFI_SUCCESS;
    CHAR8                   *LangCode;


    if (!LibInitialized) {
        LibInitialized = TRUE;
        LibFwInstance = FALSE;
        LibImageHandle = ImageHandle;

        ST = SystemTable;
        BS = SystemTable->BootServices;
        RT = SystemTable->RuntimeServices;


        if (ImageHandle) {
            BS->HandleProtocol(ImageHandle, &gEfiLoadedImageProtocolGuid, (void**) &LoadedImage);
            if (!EFI_ERROR(Status)) {
                PoolAllocationType = LoadedImage->ImageDataType;
            }
            EFIDebugVariable ();
        }
    }


    if (ImageHandle && UnicodeInterface == &LibStubUnicodeInterface) {
        LangCode = (CHAR8*)LibGetVariable( (CHAR16*)VarLanguage, (EFI_GUID*)&EfiGlobalVariable );
        InitializeUnicodeSupport (LangCode);
        if (LangCode) {
            uefi_call_wrapper(gBS->FreePool, 1, LangCode);
        }
    }

    return;
}





INTN RtCompareMem (
    IN CONST VOID     *Dest,
    IN CONST VOID     *Src,
    IN UINTN    len
    )
{
    CONST CHAR8* dst = (CONST CHAR8*)Dest;
    CONST CHAR8* src = (CONST CHAR8*)Src;
    while (len--) {
        if (*dst != *src) {
            return *dst - *src;
        }

        dst += 1;
        src += 1;
    }

    return 0;
}


INTN
CompareMem (
    IN CONST VOID     *Dest,
    IN CONST VOID     *Src,
    IN UINTN    len
    )
{
    return RtCompareMem (Dest, Src, len);
}


UINTN
strlena (
    IN CONST CHAR8    *s1
    )
// string length
{
    UINTN        len;

    for (len=0; *s1; s1+=1, len+=1) ;
    return len;
}


EFI_STATUS LibLocateHandle (
    IN EFI_LOCATE_SEARCH_TYPE       SearchType,
    IN EFI_GUID                     *Protocol OPTIONAL,
    IN VOID                         *SearchKey OPTIONAL,
    IN OUT UINTN                    *NoHandles,
    OUT EFI_HANDLE                  **Buffer
) {
    EFI_STATUS          Status;
    UINTN               BufferSize;

    //
    // Initialize for GrowBuffer loop
    //

    Status = EFI_SUCCESS;
    *Buffer = (EFI_HANDLE*)NULL;
    BufferSize = 50 * sizeof(EFI_HANDLE);

    //
    // Call the real function
    //

    while (GrowBuffer (&Status, (VOID **) Buffer, BufferSize)) {

        Status = uefi_call_wrapper(
			BS->LocateHandle,
			5,
                        SearchType,
                        Protocol,
                        SearchKey,
                        &BufferSize,
                        *Buffer
                        );

    }

    *NoHandles = BufferSize / sizeof (EFI_HANDLE);
    if (EFI_ERROR(Status)) {
        *NoHandles = 0;
    }

    return Status;
}


VOID InitializeUnicodeSupport (CHAR8 *LangCode)
{
    EFI_UNICODE_COLLATION_INTERFACE *Ui;
    EFI_STATUS                      Status;
    CHAR8                           *Languages;
    UINTN                           Index;
    UINTN                           Position;
    UINTN                           Length;
    UINTN                           NoHandles;
    EFI_HANDLE                      *Handles;


    LibLocateHandle (ByProtocol, &UnicodeCollationProtocol, NULL, &NoHandles, &Handles);
    if (!LangCode || !NoHandles) {
        goto Done;
    }

    for (Index=0; Index < NoHandles; Index++) {
        Status = BS->HandleProtocol(Handles[Index], &gEfiUnicodeCollationProtocolGuid, (void**) &Ui);
        if (EFI_ERROR(Status)) {
            continue;
        }


        Languages = Ui->SupportedLanguages;
        Length = strlena(Languages);
        for (Position=0; Position < Length; Position += ISO_639_2_ENTRY_SIZE) 
        {
            if (CompareMem (Languages+Position, LangCode, ISO_639_2_ENTRY_SIZE) == 0) {
                UnicodeInterface = Ui;
                goto Done;
            }
        }
    }


Done:
    if (Handles) {
        uefi_call_wrapper(gBS->FreePool, 1, Handles);
    }
    return;
}