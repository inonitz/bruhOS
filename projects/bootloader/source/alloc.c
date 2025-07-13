#include "efibind.h"
#include <bootloader/alloc.h>
#include <bootloader/err.h>

#include <bootloader/lib.h>

#include <bootloader/debug_print.h>



void* allocatePool(uint64_t size)
{
    void* addr = NULL;
    debug_printb(L"[allocatePool] Attempt %x bytes of type %x\n\r", size, (uint32_t)EfiBootServicesData);
    EFI_STATUS status = uefi_call_wrapper(gBS->AllocatePool, 3, EfiBootServicesData, size, &addr);
    if(EFI_ERROR(status)) {
        printf((const CHAR16*)L"allocatePool out of memory of type 'EfiBootServicesData' - Status Code '%s'\n\r", getErrorStr(status));
        addr = NULL;
    }
    debug_printb(L"[allocatePool] Attempt Success At %p\n\r", addr);

    return addr;
}


void freePool(void* address)
{
    uefi_call_wrapper(gBS->FreePool, 1, address);
    return;
}


void* allocatePages(
    EFI_ALLOCATE_TYPE alloctype,
    EFI_MEMORY_TYPE   memtype, 
    uint32_t          pageCount
) {
    EFI_PHYSICAL_ADDRESS addr = 0;
    EFI_STATUS           status;


    debug_printb(L"[allocatePages] Attempt %x pages of type %x\n\r", pageCount, (uint32_t)memtype);
    status = uefi_call_wrapper(gBS->AllocatePages, 4, 
        alloctype, 
        memtype, 
        pageCount, 
        &addr
    );
    if(EFI_ERROR(status)) {
        printf((const CHAR16*)L"[allocatePages] out of memory of type %X - Status Code %x\n", memtype, status);
        addr = 0;
    }
    debug_printb(addr == 0 ? 
        L"[allocatePages] Attempt Failure at %p\n\r" 
        : 
        L"[allocatePages] Attempt Success At %p\n\r", 
        addr
    );

    return (void*)( (uintptr_t)addr );
}


void freePages(void* address, uint32_t pageCount)
{
    uefi_call_wrapper(gBS->FreePages, 2, 
        (EFI_PHYSICAL_ADDRESS)( (uintptr_t)address ), 
        pageCount
    );
    return;
}
