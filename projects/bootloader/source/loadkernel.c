#include <bootloader/loadkernel.h>
#include <bootloader/elfutil.h>
#include <bootloader/file.h>
#include <bootloader/string.h>
#include <bootloader/err.h>
#include <bootloader/debug_print.h>
#include <bootloader/alloc.h>
#include <bootloader/pause.h>
#define __KERNEL_PREBOOT_RESOLVE_TYPE_CONFLICTS_WITH_EFI__
#include <preboot/kheader.h>
#undef __KERNEL_PREBOOT_RESOLVE_TYPE_CONFLICTS_WITH_EFI__
#include <bootloader/bool_macro.h>


void* loadKernel(
    IN  EFI_FILE_HANDLE     e_handle, 
    IN  uint64_t            cpu_core_count,
    OUT loadedElf64Program* program_config
) {
    Elf64_Ehdr  ehdr;
    Elf64_Xword tmpsize = 0;
    Elf64_Xword kernelVirtualStartAddress    = 0;
    Elf64_Xword sectionHeaderTableEntryCount = 0;
    Elf64_Xword symbolTableEntryCount        = 0;
    Elf64_Xword programHeaderTableEntryCount = 0;
    Elf64_Xword programHeaderLoadSectionsTotalSizeBytes = 0;
    uint64_t    kernelPerCoreStackArrayTotalSizeBytes = __KERNEL_PERCPU_STACK_SIZE * cpu_core_count;
    uint64_t    kernelGlobalGDTSizeBytes              = __KERNEL_GDT_ENTRY_SIZE * (5 + cpu_core_count);
    uint64_t    kernelPerCoreTSSArrayTotalSizeBytes   = __KERNEL_PERCPU_TSS_x86_64_SIZE * cpu_core_count;
    uint64_t    kernelTotalSizeBytes                  = 0;
    uint64_t    kernelTotalSizePages                  = 0;
    uint64_t    kernelEndOffset                       = 0;
    Elf64_Shdr* sectionHeaderTable       = (Elf64_Shdr*)NULL;
    Elf64_Shdr* kernelStackSectionHeader = (Elf64_Shdr*)NULL;
    Elf64_Shdr* symbolTableSectionHeader = (Elf64_Shdr*)NULL;
    Elf64_Shdr* stringTableSectionHeader = (Elf64_Shdr*)NULL;
    Elf64_Sym*  symbolTable              = (Elf64_Sym*)NULL;
    char*       sectionHeaderStringTable = (char*)NULL;
    char*       stringTable              = (char*)NULL;
    Elf64_Phdr* programHeaderTable       = (Elf64_Phdr*)NULL;
    Elf64_Phdr* lastPTLoadSegmentHdr     = (Elf64_Phdr*)NULL;
    Elf64_Addr  PHYSICAL kernelAddress   = (Elf64_Addr)NULL;
    const char  kernelStackSectionHeaderName[]  = ".stack";
    const char  symbolTableSectionHeaderName[]  = ".symtab";
    const char  stringTableSectionHeaderName[]  = ".strtab";
    const char  kernelVirtualStartAddressName[] = "__kernel_start";
    const char* currentSectionHeaderName        = (const char*)NULL;
    const char* currentSymbolName               = (const char*)NULL;
    BOOLEAN     found[3] = { FALSE, FALSE, FALSE };


    /* Stage 1 */
    readFile(
        e_handle,
        0,
        sizeof(Elf64_Ehdr),
        (uint8_t*)&ehdr,
        &tmpsize
    );
    CHECKERRFMT( tmpsize != sizeof(Elf64_Ehdr) ? -1 : 1, 
        L"\n\r    Couldn't load Elf64 File - Read Operation Failed\n\r"
    )
    CHECKERRFMT((verifyElfHeader(&ehdr) == FALSE) ? -1 : 1,
        L"\n\r    Couldn't load Elf64 File -> Header is invalid\n\r"
    )
    debug_printb(L"    Elf64 header verified successfully\n\r");
    
    
    /* Stage 2 */
    sectionHeaderTableEntryCount = elfSectionHeaderTableEntryCount(&ehdr);
    elfSectionHeaderTable(e_handle, &ehdr, &sectionHeaderTable);
    elfSectionHeaderStringTable(e_handle, &ehdr, sectionHeaderTable, &sectionHeaderStringTable);


    for(Elf64_Xword i = 0; i < sectionHeaderTableEntryCount; ++i) {
        currentSectionHeaderName = sectionHeaderStringTable + sectionHeaderTable[i].sh_name;
        if(sectionHeaderTable[i].sh_name == 0) {
            continue;
        }

        debug_printb(L"    Section [%u] -> %a\n\r", i, currentSectionHeaderName);


        found[0] = !strcmp(kernelStackSectionHeaderName, currentSectionHeaderName);
        found[1] = !strcmp(symbolTableSectionHeaderName, currentSectionHeaderName);
        found[2] = !strcmp(stringTableSectionHeaderName, currentSectionHeaderName);
        kernelStackSectionHeader = found[0] ? &sectionHeaderTable[i] : kernelStackSectionHeader;
        symbolTableSectionHeader = found[1] ? &sectionHeaderTable[i] : symbolTableSectionHeader;
        stringTableSectionHeader = found[2] ? &sectionHeaderTable[i] : stringTableSectionHeader;
    }
    CHECKERRFMT(kernelStackSectionHeader == NULL ? -1 : 1, 
        L"Couldn't Find %a Section\n\r", 
        kernelStackSectionHeaderName
    );
    CHECKERRFMT(symbolTableSectionHeader == NULL ? -1 : 1, 
        L"Couldn't Find %a Section\n\r", 
        symbolTableSectionHeaderName
    );
    CHECKERRFMT(stringTableSectionHeader == NULL ? -1 : 1, 
        L"Couldn't Find %a Section\n\r", 
        symbolTableSectionHeaderName
    );
    symbolTableEntryCount = symbolTableSectionHeader->sh_size / symbolTableSectionHeader->sh_entsize;

    debug_printb(L"    Kernel Stack Section-Header Data:\n\r    Address: %p (%X Bytes)\n\r",
        kernelStackSectionHeader->sh_addr,
        kernelStackSectionHeader->sh_size
    );
    debug_printb(L"    Symbol Table Section-Header Data:\n\r    Address: %p (%X Bytes) | Entries: %X (%X Bytes per entry)\n\r",
        symbolTableSectionHeader->sh_addr,
        symbolTableSectionHeader->sh_size,
        symbolTableEntryCount,
        symbolTableSectionHeader->sh_entsize
    );
    debug_printb(L"    String Table Section-Header Data:\n\r    Address: %p (%X Bytes)\n\r",
        stringTableSectionHeader->sh_addr,
        stringTableSectionHeader->sh_size
    );


    /* Stage 3 - now we get '__kernel_start' */
    symbolTable = (Elf64_Sym*)allocatePool(symbolTableSectionHeader->sh_size);
    stringTable = (char*)allocatePool(stringTableSectionHeader->sh_size);
    readFile(
        e_handle,
        symbolTableSectionHeader->sh_offset,
        symbolTableSectionHeader->sh_size,
        (uint8_t*)symbolTable,
        &tmpsize
    );
    CHECKERRFMT(tmpsize != symbolTableSectionHeader->sh_size ? -1 : 1,
        L"    Couldn't Read Symbol Table. Only read %X/%X Bytes\n\r",
        tmpsize,
        symbolTableSectionHeader->sh_size
    );
    readFile(
        e_handle,
        stringTableSectionHeader->sh_offset,
        stringTableSectionHeader->sh_size,
        (uint8_t*)stringTable,
        &tmpsize
    );
    CHECKERRFMT(tmpsize != stringTableSectionHeader->sh_size ? -1 : 1,
        L"    Couldn't Read String Table. Only read %X/%X Bytes\n\r",
        tmpsize,
        stringTableSectionHeader->sh_size
    );
    

    found[0] = FALSE;
    for(Elf64_Xword i = 0; i < symbolTableEntryCount; ++i) {
        currentSymbolName = &stringTable[symbolTable[i].st_name];
        if(symbolTable[i].st_name == 0) {
            continue;
        }
        if(__builtin_expect(found[0], FALSE)) {
            break;
        }

        // debug_printb(L"    [%u] st_name[%u] -> %a\n\r", i, symbolTable[i].st_name, currentSymbolName);

        found[0] = !strcmp(kernelVirtualStartAddressName, currentSymbolName);
        kernelVirtualStartAddress = found[0] ? symbolTable[i].st_value : kernelVirtualStartAddress;
    }
    CHECKERRFMT(kernelVirtualStartAddress == 0 ? -1 : 1, 
        L"    Couldn't Find symbol '%a' \n\r",
        kernelVirtualStartAddressName
    );
    debug_printb(L"    Found Symbol '%a' -> %p\n\r", kernelVirtualStartAddressName, kernelVirtualStartAddress);


    /* Stage 4 */
    programHeaderTableEntryCount = elfProgramHeaderTableEntryCount(&ehdr);
    elfProgramHeaderTable(e_handle, &ehdr, &programHeaderTable);
    for(Elf64_Xword i = 0; i < programHeaderTableEntryCount; ++i) 
    {
        programHeaderLoadSectionsTotalSizeBytes += 
            (programHeaderTable[i].p_type == PT_LOAD) 
            ?
            programHeaderTable[i].p_memsz : 0;
    }
    kernelTotalSizeBytes = 0 
        + programHeaderLoadSectionsTotalSizeBytes
        + kernelGlobalGDTSizeBytes
        + kernelPerCoreStackArrayTotalSizeBytes 
        + kernelPerCoreTSSArrayTotalSizeBytes
        + sizeof(kernel_header_t);

    tmpsize = kernelTotalSizeBytes >> LOG2_PAGE_SIZE;
    kernelTotalSizePages = tmpsize + blboolean(tmpsize & (PAGE_SIZE - 1));

    /* Kernel File Memory Allocation */
    kernelAddress = (Elf64_Addr)allocatePages(
        AllocateAnyPages, 
        (EFI_MEMORY_TYPE)BOOT_ELF_MEM_TYPE,
        kernelTotalSizePages
    );
    CHECKERRFMT(kernelAddress == 0 ? -1 : 1, 
        L"    Couldn't Allocate Elf64 File of Size %X Bytes\n\r",
        kernelTotalSizePages << LOG2_PAGE_SIZE
    );

    /* Set Zero All Kernel Memory before writing to it */
    memsetzerovoid((void*)kernelAddress, kernelTotalSizePages << LOG2_PAGE_SIZE);
    debug_printb(L"    Allocated @%p (%z Pages) for the kernel\n\r",
        kernelAddress,
        kernelTotalSizePages
    );


    /* Now Load each PT_LOAD program segment */
    for(Elf64_Xword i = 0; i < programHeaderTableEntryCount; ++i) 
    {
        found[0] = (programHeaderTable[i].p_type == PT_LOAD);
        if(!found[0]) {
            continue;
        }


        lastPTLoadSegmentHdr = &programHeaderTable[i]; /* you'll see y later */
        readFile(
            e_handle,
            programHeaderTable[i].p_offset,
            programHeaderTable[i].p_memsz,
            (uint8_t*)(kernelAddress + programHeaderTable[i].p_paddr),
            &tmpsize
        );
        CHECKERRFMT(tmpsize != programHeaderTable[i].p_memsz, 
            L"Couldn't Read PT_LOAD segment\n\r"
        );
        debug_printb(L"\
    PT_LOAD @PHYICAL_ADDR %p\n\r\
    p_addr: %p\n\r\
    v_addr: %p\n\r\
    align : 0x%X (Bytes)\n\r\
    offset: 0x%X (Bytes)\n\r\
    filesz: 0x%X (Bytes)\n\r\
    memsiz: 0x%X (Bytes)\n\r",
        kernelAddress + programHeaderTable[i].p_offset,
        programHeaderTable[i].p_paddr,
        programHeaderTable[i].p_vaddr,
        programHeaderTable[i].p_align,
        programHeaderTable[i].p_offset,
        programHeaderTable[i].p_filesz,
        programHeaderTable[i].p_memsz
        );
    }


    /* Stage 6 */
    memsetzerovoid(program_config, sizeof(*program_config));
    /* 
        kernelEndOffset is the end of the last section we just loaded.
        we won't be loading anything else (hence the name).
        Any memory beyond this point is designated for the static percpu structures
        and the virtually-mapped kernel header
    */
    /*
        [NOTE]: Triple Verify Structure Offsets!!!
    */
    kernelEndOffset = lastPTLoadSegmentHdr->p_paddr + lastPTLoadSegmentHdr->p_memsz;
    program_config->startp = kernelAddress;
    program_config->startv = kernelVirtualStartAddress;
    program_config->totalSize = kernelTotalSizeBytes;
    program_config->loadSize  = kernelTotalSizePages << LOG2_PAGE_SIZE;
    program_config->endOffset = kernelEndOffset;
    program_config->stackSection      = kernelStackSectionHeader->sh_addr; 
    program_config->stackSectionSize  = kernelStackSectionHeader->sh_size;
    program_config->percpuStackOffset = program_config->startp + kernelEndOffset;
    program_config->percpuTSSOffset   = program_config->percpuStackOffset + kernelPerCoreStackArrayTotalSizeBytes;
    program_config->GDTOffset         = program_config->percpuTSSOffset + kernelPerCoreTSSArrayTotalSizeBytes;
    program_config->postbootServicesHeader = VIRTUAL (program_config->GDTOffset + kernelGlobalGDTSizeBytes);
        debug_printb(L"Program Config > \n\r\
    addr: %p (Physical)\n\r\
    size: %X (Bytes) \n\r\
    load_size :   %z (Pages) \n\r\
    end_offset:   %X (Bytes)\n\r\
    percpu_stack @%p (Physical)\n\r\
    percpu_tss   @%p (Physical)\n\r\
    percpu_gdt   @%p (Physical)\n\r\
    postboot_hdr @%p (Physical)\n\r",
        program_config->startp,
        program_config->totalSize,
        program_config->loadSize >> LOG2_PAGE_SIZE,
        program_config->endOffset,
        program_config->percpuStackOffset,
        program_config->percpuTSSOffset,
        program_config->GDTOffset,
        program_config->postbootServicesHeader
    );


    /* Free Memory from all stages */
    /*
        [NOTE]: Triple Verify Memory Frees!!!
    */
    freePool(programHeaderTable);
    freePool(stringTable);
    freePool(symbolTable);
    freePool(sectionHeaderStringTable);
    freePool(sectionHeaderTable);
    return (void*)ehdr.e_entry;
}