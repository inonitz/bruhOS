#include "efibind.h"
#include <bootloader/memorymap.h>
#include <bootloader/lib.h>

#include <bootloader/string.h>
#include <bootloader/print.h>
#include <bootloader/err.h>
#include <bootloader/pause.h>


UINTN initializeKernelMemoryMap(efi_memory_map* map)
{
    UINTN      memoryMapKey      = 0;
    UINT32     descriptorVersion = 0;
    EFI_STATUS status            = EFI_SUCCESS;

    *map = (efi_memory_map){ (efi_mem_descriptor*)(NULL), 0, 0, 0, };
    status = uefi_call_wrapper(gBS->GetMemoryMap, 5,
        &map->used_size,
        (EFI_MEMORY_DESCRIPTOR*)NULL,
        (UINTN*)NULL,
        &map->entry_size,
        &descriptorVersion
    );


    /* Allocate Enough memory for the coming memory map + any entries we'd add later */
    // map_size will remain the same due to the next getMemoryMap call 
    map->used_size *= 2;
    map->map_size   = map->used_size;
    status = gBS->AllocatePool(
        (EFI_MEMORY_TYPE)BOOT_MEMORY_MAP_TYPE, 
        map->map_size, 
        (void**)&map->mmap
    );
    CHECKERRFMT(status, L"Couldn't Allocate a Memory Pool of size %u Bytes (Type %X)\n\r", 
        map->map_size, 
        BOOT_MEMORY_MAP_TYPE
    );
    memsetzero((uint8_t*)map->mmap, map->map_size);


    /* actually fill out the memory map */
    status = gBS->GetMemoryMap(
        &map->used_size,
        (EFI_MEMORY_DESCRIPTOR*)map->mmap,
        &memoryMapKey,
        &map->entry_size,
        &descriptorVersion
    );
    CHECKERRFMT(status, L"Couldn't Get The UEFI Memory Map.\n\r")
    
    
    // ondebug(
    //     printMemoryMap(map);
    // )
    return memoryMapKey;
}


typedef struct __attribute__((aligned(8))) id_to_string_descriptor
{
    const uint32_t id;
    const CHAR16*  str;
} id_string;


static const CHAR16* memoryTypeString(uint32_t stringID) 
{
    static const uint8_t   mem_string_len = 18;
    static const id_string mem_string[18] = {
        { 0,                    (const CHAR16*)L"EfiReservedMemoryType     " },
        { 1,                    (const CHAR16*)L"EfiLoaderCode             " },
        { 2,                    (const CHAR16*)L"EfiLoaderData             " },       
        { 3,                    (const CHAR16*)L"EfiBootServicesCode       " }, 
        { 4,                    (const CHAR16*)L"EfiBootServicesData       " }, 
        { 5,                    (const CHAR16*)L"EfiRuntimeServicesCode    " },
        { 6,                    (const CHAR16*)L"EfiRuntimeServicesData    " },
        { 7,                    (const CHAR16*)L"EfiConventionalMemory     " }, 
        { 8,                    (const CHAR16*)L"EfiUnusableMemory         " },
        { 9,                    (const CHAR16*)L"EfiACPIReclaimMemory      " }, 
        { 10,                   (const CHAR16*)L"EfiACPIMemoryNVS          " },
        { 11,                   (const CHAR16*)L"EfiMemoryMappedIOPortSpace" },
        { 12,                   (const CHAR16*)L"EfiPalCode                " },
        { 13,                   (const CHAR16*)L"EfiPersistentMemory       " },
        { 14,                   (const CHAR16*)L"EfiMaxMemoryType          " },
        { BOOT_ELF_MEM_TYPE,    (const CHAR16*)L"EfiOSLoaderLoadedImage    " },
        { BOOT_PAGING_MEM_TYPE, (const CHAR16*)L"EfiOSLoaderPagingData     " },
        { BOOT_MEMORY_MAP_TYPE, (const CHAR16*)L"EfiOSLoaderMemoryMap      " }
    };
    
    uint8_t idx = 0;
    for(; idx < mem_string_len && stringID != mem_string[idx].id; ++idx) {}
    return mem_string[idx].str;
}


#define MAP_ENTRY_AT(MAP, i) (efi_mem_descriptor*)((uint64_t)(MAP)->mmap + ((uint64_t)(i) * (MAP)->entry_size))  


#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function"
void dumpMemoryMap(efi_memory_map* map)
{
    const uint64_t      dscs = map->used_size / map->entry_size;
    efi_mem_descriptor* dsc  = 0;

    (void)(dscs);
    // pauseforinput();
    printf((const CHAR16*)L"EFI Memory Map Description:\n\r");
    printf((const CHAR16*)L"  Map Size/Used/EntrySize: 0x%x/0x%x/%x\n\r", map->map_size, map->used_size, map->entry_size);
    for(uint16_t i = 0; i < dscs; ++i)
    {
        // pauseforinput();
        dsc = MAP_ENTRY_AT(map, i);
        printf((const CHAR16*)L"  %s | %p | %p | %u\n\r", 
            memoryTypeString(dsc->memtype),
            dsc->physAddr, 
            dsc->virtAddr, 
            dsc->pages
        );
        // pauseforinput();
        // pause(250000);
    }


    return;
}
#pragma GCC diagnostic pop


#undef MAP_ENTRY_AT
