#include <bootloader/lib.h>
#define __KERNEL_PREBOOT_RESOLVE_TYPE_CONFLICTS_WITH_EFI__
#include <bootloader/acpi.h>
#define __KERNEL_PREBOOT_RESOLVE_TYPE_CONFLICTS_WITH_EFI__


#include <bootloader/err.h>
#include <bootloader/debug_print.h>
#include <bootloader/bool_macro.h>

#include <preboot/uefi.h>



void get_verify_acpi_rsdp(OUT RSDPdescriptor20** dsc)
{
    EFI_STATUS status      = EFI_SUCCESS;
    EFI_GUID   acpi20_guid = ACPI_20_TABLE_GUID;
    EFI_GUID   acpi10_guid = ACPI_TABLE_GUID;
    uint8_t*   tmp         = 0;
    uint8_t    checksum    = 0;

    printb(L"\n\rProbing ACPI Tables...\n\r");
    status = LibGetSystemConfigurationTable(&acpi20_guid, (void**)&tmp);
    if(status == EFI_NOT_FOUND) {
        printb(L"    Couldn't find ACPI_2.0+, Using ACPI_1.0 instead.\n\r");
        status = LibGetSystemConfigurationTable(&acpi10_guid, (void**)&tmp);


    }
    

    printb(L"    Found RSDP Descriptor at address %p in memory\n\r    ", tmp);
    for(uint8_t i = 0; i < (uint8_t)sizeof(RSDPdescriptor20); ++i)
    {
        checksum += tmp[i];
        debug_printb(L"%x ", tmp[i]);
        if(!blboolean(i % 5) && i != 0)
            debug_printb(L"\n\r    ");
    }
    CHECKERRFMT(checksum, L"    RSDP descriptor Checksum Failed, Table is invalid. Sum: %u\n\r", checksum)
    printb(L"\n\r    RSDP Descriptor for ACPI_v%u Table Checksum Verified Successfully\n\r", ((RSDPdescriptor20*)tmp)->first.revision);


    *dsc = (RSDPdescriptor20*)tmp;
    return;
}


uint32_t get_core_count(void)
{
    // the CPU Cores need Individual Structures in memory, such as stacks.
    // We'll find the Core count and allocate accordingly.
    EFI_STATUS                       status  = EFI_SUCCESS;
    EFI_GUID                         mpsvsg  = EFI_MP_SERVICES_PROTOCOL_GUID;
    Efi_mp_services*                 mpsvs   = (Efi_mp_services*)NULL;
    uint64_t                         cores   = 0;
    uint64_t                         enabled = 0;
    

    printb(L"Finding Core Count through MP Services...\n\r");
    status = uefi_call_wrapper(gBS->LocateProtocol, 3, &mpsvsg, NULL,(void**)&mpsvs);
    CHECKERRFMT_NOEXIT(status, L"   get_core_count() -> MP Services aren't available\n\r");

    status = mpsvs->GetNumberOfProcessors(mpsvs, &cores, &enabled);
    CHECKERRFMT_NOEXIT(status, L"   get_core_count() -> GetNumberOfProcessors() Failed\n\r");


    printb(L"    Detected Processor Core-Count is %u\n\r", cores);
    return (uint32_t)cores;
}