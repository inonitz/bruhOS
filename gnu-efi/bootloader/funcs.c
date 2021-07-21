#include "funcs.h"
#include <elf.h>
#include "../../shared/uefi.h"



#define TSS_X86_64_SIZE 0x68
#define GDT_ENTRY_SIZE  0x08
#define AP_CORE_STACK_SIZE (8 * EFI_PAGE_SIZE)



void findFileInternal(EFI_FILE_HANDLE current_dir, CHAR16* filename, EFI_FILE_HANDLE* out)
{
    EFI_STATUS     status      = EFI_SUCCESS;
    EFI_FILE_INFO* fileinf;
    uint64_t       fileinfsize = 1;


    while (fileinfsize != 0)
    {
        fileinfsize = 1;                                               // reset fileinfsize to retrieve the next file/dir info.
        status = current_dir->Read(current_dir, &fileinfsize, NULL);   // get size of info; if current_dir is directory: reading returns EFI_FILE_INFO to first file.
        if (fileinfsize == 0) continue;                                // if fileinfsize == 0: => we reached the end of the directory.

        fileinf = AllocatePool(fileinfsize);                           // allocate buffer for info
        status = current_dir->Read(current_dir, &fileinfsize, fileinf);// get info in fileinf

        ondebug(
            Print(L"%s %u %u\n", fileinf->FileName, fileinfsize, !!(fileinf->Attribute & EFI_FILE_DIRECTORY));
            // gBS->Stall(500000);
        );


        if (StrCmp(fileinf->FileName, L".") == 0 || StrCmp(fileinf->FileName, L"..") == 0)
            // . = current_dir, .. = parent_dir. we don't need to search them.
            continue;

        if (!!!(fileinf->Attribute & EFI_FILE_DIRECTORY))
        {
            // if we found our file, return a handle to it.
            if (StrCmp(fileinf->FileName, filename) == 0)
            {
                Print(L"Found file %s! returning...\n", filename);
                status = current_dir->Open(current_dir, out, filename, EFI_FILE_MODE_READ, 0);
                CHECKERR(status, Print(L"\n Couldn't open handle to desired file!\n"))
                FreePool(fileinf);
                return;
            }
            FreePool(fileinf);
            continue; // else continue to the next file
        }
        else
        {
            // search in new directory.
            EFI_FILE_HANDLE newDir;
            status = current_dir->Open(current_dir, &newDir, fileinf->FileName, EFI_FILE_MODE_READ, 0);
            CHECKERR(status, Print(L"Couldn't open directory %s\n", fileinf->FileName));

            status = newDir->SetPosition(newDir, 0);
            findFileInternal(newDir, filename, out);

            status = current_dir->Close(newDir);
            CHECKERR(status, Print(L"Couldn't close file %s\n", fileinf->FileName));
            FreePool(fileinf);
        }
    }
}



const CHAR16* pixel_formats[5] = {
    L"RedGreenBlueReserved8BitPerColor",
    L"BlueGreenRedReserved8BitPerColor",
    L"BitMask",
    L"BltOnly",
    L"FormatMax"
};

void PrintAvailableVideoModes(IN EFI_GRAPHICS_OUTPUT_PROTOCOL* gop)
{
	EFI_STATUS							  status = EFI_SUCCESS;
	EFI_GRAPHICS_OUTPUT_MODE_INFORMATION* gop_mode;
    uint64_t                              infSize;
    uint32_t                              modecount;


	status = gop->QueryMode(gop, gop->Mode->Mode, &infSize, &gop_mode);
	CHECKERR(status);

	modecount = gop->Mode->MaxMode;
    Print(L"Printing %u Available Video Modes... Current Mode is: %u.\n", modecount, gop->Mode->Mode);
    
	for (uint32_t i = 0; i < modecount; ++i) {
		status = gop->QueryMode(gop, i, &infSize, &gop_mode);
		CHECKERR(status);
		Print(L"    Mode %02d - <%u, %u> pxfmt: %s\n",
			i,
			gop_mode->HorizontalResolution,
			gop_mode->VerticalResolution,
            pixel_formats[gop_mode->PixelFormat]
		);
		// gBS->Stall(100000 * 2);
	}
}




void setVideoMode(
    IN EFI_GRAPHICS_OUTPUT_PROTOCOL* gop,
    IN uint16_t                      mode
)
{
    gop->SetMode(gop, (uint32_t)mode);
}




void initialize_screenbuffer(
    IN  vec2us                        dimensions,
    OUT framebuffer_t*                init
)
{
    EFI_STATUS                            status     = EFI_SUCCESS;
    EFI_GRAPHICS_OUTPUT_PROTOCOL*         gop        = NULL;
    EFI_GRAPHICS_OUTPUT_MODE_INFORMATION* gop_mode   = NULL;
    uint64_t                              infSize    = 0;
    uint32_t                              modecount  = 0;
    uint32_t                              i          = 0;
    bool_t                                found_mode = FALSE;


    Print(L"\nInitializing Graphics Framebuffer... \n");

    // locate the gop protocol
    status = gBS->LocateProtocol(&gEfiGraphicsOutputProtocolGuid, NULL, (void**)&gop);
    CHECKERR(status, Print(L"   Couldn't get an active screen protocol (GRAPHICS_OUTPUT_PROTOCOL)\n");)
    ondebug(PrintAvailableVideoModes(gop);)


    // get the current mode GOP is set to
	status = gop->QueryMode(gop, gop->Mode->Mode, &infSize, &gop_mode);
	CHECKERR(status, Print(L"   couldn't query current video mode in order to initialize screen framebuffer\n"));
    
    dimensions.x = (dimensions.x == 0) * 1920u + (dimensions.x != 0) * dimensions.x;
    dimensions.y = (dimensions.y == 0) * 1080u + (dimensions.y != 0) * dimensions.y; 
    if(gop_mode->HorizontalResolution == dimensions.x && gop_mode->VerticalResolution == dimensions.y) {
        Print(L"    Current GRAPHICS_OUTPUT_PROTOCOL configuration is already set to %ux%u.\n", dimensions.x, dimensions.y);
        found_mode = TRUE;
        i          = gop->Mode->Mode;
    }   
    else {
        Print(L"    Framebuffer isn't set to %ux%u, searching for possible configuration...\n", dimensions.x, dimensions.y);
        // gBS->Stall(100000 * 50);
        modecount = gop->Mode->MaxMode;
        for (; i < modecount && !found_mode; ++i) {
            status = gop->QueryMode(gop, i, &infSize, &gop_mode);
            CHECKERR(status);
            found_mode = gop_mode->HorizontalResolution == dimensions.x && gop_mode->VerticalResolution == dimensions.y;
        }
        Print(L"    Framebuffer initialized Successfully to %ux%u resolution.\n", init->dims.x, init->dims.y);
    }
    
    
    ondebug({
        status = gop->SetMode(gop, 0); // reset the video mode.
        CHECKERR(status, Print(L"   DEBUG: Couldn't reset video mode\n");)
    });


    status = gop->SetMode(gop, found_mode * i);      // if we found it, set to dimensions.xy. else, set to mode 0 (default).
    CHECKERR(status, Print(L"   Couldn't change to appropriate video mode %u\n", found_mode * i);)


    init->start = (void*)gop->Mode->FrameBufferBase; // get address of the framebuffer
    init->dims  = (vec2us){ gop->Mode->Info->HorizontalResolution, gop->Mode->Info->VerticalResolution };

    ondebug(Print(L"    Framebuffer Physical Address: %X\n", init->start);)
    return;
}




EFI_FILE_HANDLE findFile(EFI_FILE_HANDLE Directory, CHAR16* filename)
{
    EFI_STATUS      status   = EFI_SUCCESS;
    EFI_FILE_HANDLE out      = NULL;
    if (Directory == NULL)
    {
        EFI_SIMPLE_FILE_SYSTEM_PROTOCOL* FileSys = NULL;

        status = gBS->LocateProtocol(&gEfiSimpleFileSystemProtocolGuid, NULL, (VOID**)&FileSys);
        CHECKERR(status, Print(L"Couldn't find active protocols for a file system.\n"))
        status = FileSys->OpenVolume(FileSys, &Directory);
        CHECKERR(status, Print(L"Couldn't Open File System\n"))
    }
    findFileInternal(Directory, filename, &out);
    return out;
}



typedef struct __id_to_string_descriptor
{
    const uint32_t id;
    const CHAR16*  str;
} id_string;


id_string* memoryTypeString(uint32_t id, id_string* arr, uint8_t len) {
    uint8_t i = 0;
    while(arr->id != id && i < len) {
        ++arr;
        ++i;
    }
    return arr;
}


#define ENTRY_AT(MAP, i) (efi_mem_descriptor*)((uint64_t)header->map.mmap + ((uint64_t)(i) * header->map.entry_size))         
#define MAP_ENTRY_AT(MAP, i) (efi_mem_descriptor*)((uint64_t)MAP->mmap + ((uint64_t)(i) * MAP->entry_size))         


void PrintMemMapDescriptors(efi_memory_map* map)
{
    const uint64_t      dscs = map->map_size / map->entry_size;    
    efi_mem_descriptor* dsc  = 0;

    id_string mem_string[18] = {
        { 0,                    L"EfiReservedMemoryType     " },
        { 1,                    L"EfiLoaderCode             " },
        { 2,                    L"EfiLoaderData             " },       
        { 3,                    L"EfiBootServicesCode       " }, 
        { 4,                    L"EfiBootServicesData       " }, 
        { 5,                    L"EfiRuntimeServicesCode    " },
        { 6,                    L"EfiRuntimeServicesData    " },
        { 7,                    L"EfiConventionalMemory     " }, 
        { 8,                    L"EfiUnusableMemory         " },
        { 9,                    L"EfiACPIReclaimMemory      " }, 
        { 10,                   L"EfiACPIMemoryNVS          " },
        { 11,                   L"EfiMemoryMappedIOPortSpace" },
        { 12,                   L"EfiPalCode                " },
        { 13,                   L"EfiPersistentMemory       " },
        { 14,                   L"EfiMaxMemoryType          " },
        { BOOT_ELF_MEM_TYPE,    L"EfiOSLoaderLoadedImage    " },
        { BOOT_PAGING_MEM_TYPE, L"EfiOSLoaderPagingData     " },
        { BOOT_MEMORY_MAP_TYPE, L"EfiOSLoaderMemoryMap      " }
    };
    Print(L"EFI Memory Map Description:\n");
    Print(L"Map Size: 0x%X; Used: 0x%X.\nEntry Size: 0x%X\n", map->map_size, map->used_size, map->entry_size);
    Print(L"memory type                | p          | v          | pages    ||\n");


    for(uint16_t i = 0; i < dscs; ++i)
    {
        dsc = MAP_ENTRY_AT(map, i);
        Print(L"%s | 0x%l8x | 0x%l8x | %8x ||\n", 
            memoryTypeString(dsc->memtype, &mem_string[0], 18u)->str,
            dsc->physAddr, 
            dsc->virtAddr, 
            dsc->pages
        );
        // gBS->Stall(100000);
    }
    return;
}




typedef struct __attribute__((packed)) __GenericPageTable_x86_64
{
    uint64_t entry[512];
} PageTable;


typedef struct __attribute__((packed)) __GenericTableEntry_x86_64
{
    union 
    {
        struct __attribute__((packed)) {
            uint8_t  present     : 1;
            uint8_t  rw          : 1;
            uint8_t  userOrSuper : 1;
            uint8_t  wthrough    : 1;
            uint8_t  cachedis    : 1;
            uint8_t  accessed    : 1;
            uint8_t  ignore0     : 1;
            uint8_t  pageSize    : 1; // 0 = 4KB, 1 = 4MiB
            uint8_t  ignore1     : 4;
            uint64_t address     : 40;
            uint16_t ignore2     : 11;
            uint8_t  noExec      : 1;
        };
        uint64_t ui64;
    };
} TableEntry;


typedef uint8_t page[4096];
#define ALLOC_PAGE(PAGE_STACK_POINTER) PAGE_STACK_POINTER++


void* mapPages(
    PageTable*       Table, 
    page*            pageStack,
    uint64_t         physStart, 
    uint64_t         virtStart, 
    uint64_t         flags
) {
    PageTable*  table      = (PageTable*)Table;
    TableEntry* ent        = NULL; 
    uint64_t    address    = (uint64_t)virtStart;
    uint64_t    indices[4] = {0};

    address >>= 12; indices[0] = address & 0x1ff; // The index for the page in the PageTable                                    [ PageTable[i]                 ]
    address >>= 9;  indices[1] = address & 0x1ff; // The index for the page table in the Page Directory table                   [ PageDirectory[i]             ]
    address >>= 9;  indices[2] = address & 0x1ff; // the index for the page directory table in the page directory pointer table [ PageDirectoryPointerTable[i] ]
    address >>= 9;  indices[3] = address & 0x1ff; // the index for the page directory pointer table in the PML4 table.          [ PML4[i]                      ]


    for(uint8_t i = 0; i < 3u; ++i)
    {
        ent = (TableEntry*)&table->entry[indices[3 - i]];
        if(!ent->present)
        {
            table = (PageTable*)ALLOC_PAGE(pageStack);
            SetMem((void*)table, 4096, 0x00);

            ent->address = (uint64_t)table >> 12;
            ent->present = TRUE;
            ent->rw      = TRUE;
        }
        else {
            table = (PageTable*)(((uint64_t)ent->address) << 12);
        }
    }

    ent = (TableEntry*)&table->entry[indices[0]];
    ent->address     = (uint64_t)physStart >> 12;
    ent->present     = TRUE;
    ent->userOrSuper = FALSE;
    ent->rw          = TRUE;

    return pageStack;
}




void tableInfo(PageTable* table)
{
    for(uint16_t i = 0; i < 4096u; ++i)
    {
        Print(L"%4u: %l10x ", i, table[i]);
        // gBS->Stall(2500);
        if((i+1) % 4 == 0) { Print(L"\n"); }
    }
}



// static void __print_header(kernel_header_t* hdr)
// {
//     Print(L"\nKernel Header Contents:\n");
//     Print(L"	Memory Map:\n		Address:    0x%lx\n		Used  Size: 0x%lx\n		Total Size: 0x%lx\n		Entry Size: 0x%lx\n	Memory Config:\n		physical Kernel Start: 0x%lx\n		virtual  Kernel Start: 0x%lx\n		PML4 Address:		   0x%lx\n		PML4 Size:             0x%lx\n		Kernel Stack Address:  0x%lx\n		Kernel Stack Size:     0x%lx (bytes)\n		RIP Identity Mapping:  0x%lx\n		RIP Page-Map Address:  0x%lx\n		RIP Page-Map Size:     0x%lx\n		Per-Core Stacks Start: 0x%lx\n		Per-Core TSS Segments: 0x%lx\n	Framebuffer:\n		Virtual Start: 0x%lx\n		Dimensions: %ux%u\n	ACPI Config:\n		XSDT(? => bool(%u)) Physical Address: 0x%lx\n		Processor Count: 					  %u\n\n	GDT Virtual Address: 				  0x%lx\n	EFI Runtime Services Virtual Address: 0x%lx\n",
//         hdr->map.mmap,
//         hdr->map.used_size,
//         hdr->map.map_size,
//         hdr->map.entry_size,

//         hdr->memcfg.physicalAddress,
//         hdr->memcfg.virtualAddress,
//         hdr->memcfg.addressPML4,
//         hdr->memcfg.sizePML4,
//         hdr->memcfg.rbpAddress,
//         hdr->memcfg.rbpSize,
//         hdr->memcfg.ident_rip,
//         hdr->memcfg.ident_rip_addr,
//         hdr->memcfg.ident_rip_size,
//         hdr->memcfg.percpuRBP,
//         hdr->memcfg.percpuTSS,

//         hdr->screen.start,
//         hdr->screen.dims.x,
//         hdr->screen.dims.y,

//         hdr->acpi.extended,
//         hdr->acpi.address,
//         hdr->acpi.procCount,

//         hdr->gdt,
//         hdr->efi_rt
//     );
// }




void __attribute__((noreturn)) enablePagingJumpToElf64Ver3(
    kernel_header_t* header,
    kernel_header_t* hHalfHdr,
    __kernel_entry   entry, 
    uint64_t         physStart, 
    uint64_t         virtStart
) {
    const uint16_t      dscs        = (uint16_t)(header->map.used_size / header->map.entry_size);    
    efi_mem_descriptor* dsc         = header->map.mmap;

    uint64_t            toVirtual   = virtStart - physStart; // where physical memory starts now.
    PageTable*          PML4;

    uint64_t            rip         = 0;
    uint64_t            fbSize      = 0;
    kernel_header_t*    newHeader   = 0;

    efi_mem_descriptor* biggestFree = header->map.mmap;
    uint64_t            biFreePages = 0;
    uint16_t            biFreeIdx   = 0;
    bool_t              usable      = 0;
    efi_mem_descriptor* prev;
    efi_mem_descriptor* pageTables;
    efi_mem_descriptor* leftOverFree;

    page*               pageStack;
    page*               ident_rip;
    

    // fix virtual addresses inside the memory map.
    for(uint16_t i = 0; i < dscs; ++i)
    {
        dsc           = ENTRY_AT(header, i);
        dsc->virtAddr = dsc->physAddr + toVirtual * !(dsc->memtype == EfiRuntimeServicesCode || dsc->memtype == EfiRuntimeServicesData);
        // find the biggest free region for allocation. we can fairly assume there is enough RAM.
        usable        = (dsc->memtype == EfiConventionalMemory) || (dsc->memtype == EfiBootServicesCode) || (dsc->memtype == EfiBootServicesData);
        usable       *= (dsc->pages > biggestFree->pages);
        biFreeIdx     = i * usable + biFreeIdx * !usable;
        biggestFree   = (efi_mem_descriptor*)( (uint64_t)dsc * usable + (uint64_t)biggestFree * !usable );

        // if(dsc->memtype == BOOT_ELF_MEM_TYPE)
        // {
        //     __asm__ volatile(
        //         "mov %0, %%r12\n\t"
        //         "mov %1, %%r13\n\t"
        //         "mov %2, %%r14\n\t"
        //         "mov %3, %%r15\n\t"
        //         "hlt" : : "r"(dsc->pages), "r"(dsc->physAddr), "r"(dsc->virtAddr), "r"(toVirtual)
        //     );
        // }
    }
    biFreePages = biggestFree->pages;


    // map the whole address space, besides the paging tables descriptor.
    pageStack = (page*)biggestFree->physAddr;
    PML4      = (PageTable*)ALLOC_PAGE(pageStack);
    SetMem(PML4, 4096, 0x00);
    for(uint16_t i = 0; i < dscs; ++i)
    {
        dsc = ENTRY_AT(header, i);
        for(uint64_t pc = 0; pc < dsc->pages; ++pc) {
            pageStack = mapPages(PML4, pageStack, dsc->physAddr + pc * 4096, dsc->virtAddr + pc * 4096, 0);
        }        
    }


    // map the next Page-worth of instructions.
    __asm__ volatile("lea (%%rip), %0" : "=r"(rip));
    ident_rip = pageStack;
    pageStack = mapPages(PML4, ident_rip, rip, rip, 0);
    header->memcfg.ident_rip      = (void*)rip;
    header->memcfg.ident_rip_addr = ident_rip;
    header->memcfg.ident_rip_size = ((uint64_t)pageStack - (uint64_t)ident_rip) >> 12;


    // map the framebuffer to the higher half.
    fbSize = (uint64_t)header->screen.dims.x * (uint64_t)header->screen.dims.y * sizeof(uint32_t);
    for(uint64_t i = 0; i < fbSize; i += 4096) 
    {
        pageStack = mapPages(PML4, pageStack, 
            (uint64_t)header->screen.start + i, 
            (uint64_t)header->screen.start + toVirtual + i, 
            0
        );
    }



    // update our kernel header to a known location in virtual memory.
    // because hHalfHdr is just beside the kernel, its also mapped in virtual mem.
    CopyMem(hHalfHdr, header, sizeof(kernel_header_t));


    // Move all entries from idx biFreeIdx to the right by 1.
    for(uint16_t i = dscs; i > biFreeIdx; --i)
    {
        prev = ENTRY_AT(header, i - 1);
        dsc  = ENTRY_AT(header, i    );
        *dsc = *prev;
    }

    // update descriptor for pageTables.
    pageTables = ENTRY_AT(header, biFreeIdx);
    pageTables->memtype = BOOT_PAGING_MEM_TYPE;
    pageTables->pages   = ((uint64_t)pageStack - pageTables->physAddr) >> 12;

    // update descriptor for the left over pages inside biggestFree.
    leftOverFree = ENTRY_AT(header, biFreeIdx+1);
    leftOverFree->memtype  = EfiConventionalMemory;
    leftOverFree->physAddr = pageTables->physAddr   + pageTables->pages;
    leftOverFree->virtAddr = leftOverFree->physAddr + toVirtual;
    leftOverFree->pages    = biFreePages - pageTables->pages;

    // PrintMemMapDescriptors(&header->map);

    // remap pointers in the kernel header
    newHeader = hHalfHdr;
    newHeader->acpi.address            += toVirtual;
    newHeader->memcfg.percpuRBP        += toVirtual;
    newHeader->memcfg.percpuTSS        += toVirtual;
    newHeader->gdt                     += toVirtual;
    newHeader->map.mmap                 = (efi_mem_descriptor*)((char*)newHeader->map.mmap + toVirtual);
    newHeader->map.used_size           += newHeader->map.entry_size;
    newHeader->screen.start             = (void*)((char*)newHeader->screen.start + toVirtual);
    newHeader->memcfg.physicalAddress   = physStart;
    newHeader->memcfg.virtualAddress    = virtStart;
    newHeader->memcfg.addressPML4       = PML4;
    newHeader->memcfg.sizePML4          = pageTables->pages;
    newHeader                           = (kernel_header_t*)((uint64_t)newHeader + toVirtual);

    // __print_header(header);
    // __print_header(hHalfHdr);

    __asm__ volatile("movq %0, %%rdi\n\t" : : "r"(newHeader)); // set variables before switching paging directories
    __asm__ volatile("movq %0, %%rcx\n\t" : : "r"(entry));     // ^^^^
    __asm__ volatile("movq %0, %%cr3\n\t" : : "r" (PML4));     // switch to new paging directory.
    // __asm__ volatile("movq $0xDEADBEEFCCCCCCCC, %r15");        // < was used previously for testing that it worked.
    // __asm__ volatile("hlt");
    __asm__ volatile("jmp *%rcx\n\t");                         // jump to kernel entry with header pointer in rdi.
    pauseloop();
}




/* 
    * Verifies that the elf64 header is valid
    * returns boolean value
*/
bool_t verifyElf64Header(Elf64_Ehdr* header)
{
    unsigned char magic[4] = { 0x7f, 'E', 'L', 'F' };
    unsigned char cmp      = memcmp(header->e_ident, magic, 4ul);

    ondebug({
        Print(L"    Header Debug Info: \n");
        Print(L"        MAGIC       == {0x7f, 'ELF'}: %u\n", cmp);
        Print(L"        ei_class    == ELFCLASS64   : %u\n", header->e_ident[EI_CLASS]   == ELFCLASS64 );
        Print(L"        ei_data     == ELFDATA2LSB  : %u\n", header->e_ident[EI_DATA]    == ELFDATA2LSB); 
        Print(L"        ei_version  == EV_CURRENT   : %u\n", header->e_ident[EI_VERSION] == EV_CURRENT ); 
        Print(L"        e_type      == ET_EXEC      : %u\n", header->e_type              == ET_EXEC    );
        Print(L"        e_machine   == EM_X86_64    : %u\n", header->e_machine 		     == EM_X86_64  );
        Print(L"        e_version   == EV_CURRENT   : %u\n", header->e_version 		     == EV_CURRENT );
    });

    return cmp &&
		header->e_ident[EI_CLASS]   == ELFCLASS64  &&
		header->e_ident[EI_DATA]    == ELFDATA2LSB &&
		header->e_ident[EI_VERSION] == EV_CURRENT  &&
		header->e_type 			    == ET_EXEC     &&
		header->e_machine 			== EM_X86_64   &&
		header->e_version 			== EV_CURRENT;
}


Elf64_Shdr probeSection(
    IN EFI_FILE_HANDLE hdl, 
    IN Elf64_Ehdr*     elf_header,
    IN const char*     name,
    IN uint16_t        nameSize
) {
    Elf64_Shdr  str_tbl;
    Elf64_Shdr  curHdr;
    uint8_t*    sec_names;
    uint64_t    hSize = sizeof(Elf64_Shdr);
    bool_t      found = FALSE;

    // seek to the position of the string table.
    // read the contents of the string table into str_tbl.
    hdl->SetPosition(hdl, elf_header->e_shoff + elf_header->e_shstrndx * elf_header->e_shentsize);  
    hdl->Read(hdl, &hSize, &str_tbl);
    
    sec_names = AllocatePool(str_tbl.sh_size);   // allocate buffer for names
    hdl->SetPosition(hdl, str_tbl.sh_offset);    // goto buffer position in file
    hdl->Read(hdl, &str_tbl.sh_size, sec_names); // copy the buffer to local buffer sec_names.


    for(uint16_t sectionIdx = 0; !found && sectionIdx < elf_header->e_shnum; ++sectionIdx)
    {
        hdl->SetPosition(hdl, elf_header->e_shoff + sectionIdx * hSize);
        hdl->Read(hdl, &hSize, &curHdr);

        // some entries might not have string indices, especially first one (null).
        if(__builtin_expect(curHdr.sh_name, TRUE)) { 
            found = memcmp((uint8_t*)name, sec_names + curHdr.sh_name, nameSize);
            ondebug(Print(L"    EXPECT: %a. REALITY: %a.\n", name, sec_names + curHdr.sh_name);)
        }
    }


    FreePool(sec_names);
    CHECKERR(!found, Print(L"    COULDN'T FIND %a SECTION!\n", name));
    Print(L"    FOUND %a SECTION!\n", name);
    Print(L"        Section Address (virtual): 0x%lx\n", curHdr.sh_addr     );
    Print(L"        Section Size    (bytes)  : 0x%lx\n", curHdr.sh_size     );
    return curHdr;
}


void* loadElf64(
    EFI_FILE_HANDLE e_handle, 
    uint64_t        cpu_core_count,
    uint64_t*       loaded_address, 
    uint64_t*       load_address, 
    uint64_t*       loaded_size,
    uint64_t*       progStack,
    uint64_t*       progStackSize,
    uint64_t*       apCoreStackOff,
    uint64_t*       apTssOff,
    uint64_t*       gdtOff,
    uint64_t*       higherHalfKhdr
) {
    EFI_STATUS  status = EFI_SUCCESS;
    uint64_t    tmp    = 0;
    Elf64_Ehdr  ehdr;      // elf header 
    Elf64_Phdr* phdrtable; // program header table
    Elf64_Shdr  sSec;      // stack section-header.
    Elf64_Addr  physoff;   // physical offset of each program header


    // verify ELF header:
    tmp    = sizeof(Elf64_Ehdr);
    status = e_handle->Read(e_handle, &tmp, &ehdr);
    Print(L"\nloading ELF File... \n");
    CHECKERR(status, Print(L"   couldn't read elf64 header"))
    CHECKERR(!verifyElf64Header(&ehdr), Print(L"\n    Couldn't load Elf64 File -> Header is invalid\n"))
    Print(L"    Elf64 header verified successfully\n");
    

    // find the address of the .stack section:
    Print(L"    Searching For usable Stack Segment in the Elf File...\n");
    sSec           = probeSection(e_handle, &ehdr, ".stack", 7u);
    *progStack     = sSec.sh_addr;
    *progStackSize = sSec.sh_size;


    // parse the program headers, looking for executable sections:
    e_handle->SetPosition(e_handle, ehdr.e_phoff); // goto program headers table 
    tmp       = ehdr.e_phnum * ehdr.e_phentsize;   // get total size of the program headers table
    phdrtable = AllocatePool(tmp);                 // allocate buffer for the table
    e_handle->Read(e_handle, &tmp, phdrtable);     // read the table into the buffer

    // // get the total size of all executable sections. then allocate all needed memory (at once)
    Elf64_Addr phdrt_end  = (Elf64_Addr)phdrtable + tmp;
    Elf64_Addr ptload     = 0;
    uint64_t   kernelSize = 0;


    // total amount of pages needed to load the executable segments.
    for (Elf64_Phdr* h = phdrtable; (Elf64_Addr)h < phdrt_end; kernelSize +=  h->p_memsz * (h->p_type == PT_LOAD), ++h); 
    CHECKERR(tmp == 0, {
        Print(L"    Couldn't Load ELF64 File into memory - No PT_LOAD (Executable) Section. Exiting...\n");
    })
    


    // TSS, 32KiB Stack & GDT TSS Entry per AP Core. 
    uint64_t gdtSize     = GDT_ENTRY_SIZE * (5 + cpu_core_count);
    uint64_t totalStacks = AP_CORE_STACK_SIZE * cpu_core_count;
    uint64_t totalTSS    = TSS_X86_64_SIZE * cpu_core_count;

    // The total Amount of memory needed is:
    // SIZE = sizeof(Kernel) + sizeof(totalTSS + totalStacks + gdtSize) + sizeof(KernelHeader).
    // Order in Memory: (0)| Kernel | Stacks | TSS | GDT | Header |(SIZE)
    tmp = kernelSize + gdtSize + totalStacks + totalTSS + sizeof(kernel_header_t);
    tmp = (tmp >> EFI_PAGE_SHIFT) + !!(tmp & 0xfff); // convert to pages.


    status = gBS->AllocatePages(AllocateAnyPages, BOOT_ELF_MEM_TYPE, tmp, &ptload);
    CHECKERR(status, { 
        Print(L"    Tried to allocate %llu KB\ncouldn't load all executable sections into memory. exiting...\n", tmp * 4ul);
    })
    ondebug(Print(L"    Allocated %llu KB in address %X\n", tmp * 4ul, ptload);)


    SetMem((void*)ptload, tmp, 0x00u); // clear the buffer we just allocated.
    *loaded_address = ptload;
    *loaded_size    = tmp << EFI_PAGE_SHIFT;
    *apCoreStackOff = ptload + *loaded_size;
    *apTssOff       = *apCoreStackOff + totalStacks;
    *gdtOff         = *apTssOff + totalTSS;
    *higherHalfKhdr = *gdtOff + gdtSize;
    ondebug(
        Print(L"loaded_address: %lx\nloaded_size:    %lx\napCoreStackOff: %lx\napTssOff:		%lx\nGdtOff:			%lx\nhigherHalfKhdr: %lx\n",
            *loaded_address,
            *loaded_size,
            *apCoreStackOff,
            *apTssOff,
            *gdtOff,
            *higherHalfKhdr
        );
    )




    // initialize OUT variables.
    // 1. get symbol table.
    // 2. get string table.
    // 3. if string_table[symbol.name] == "__kernel_start"
    // 4.   set loaded_address to symbol.value;
    Elf64_Shdr symtab   = probeSection(e_handle, &ehdr, ".symtab", 8);
    Elf64_Shdr strtab   = probeSection(e_handle, &ehdr, ".strtab", 8);
    Elf64_Sym* symbols  = AllocatePool(symtab.sh_size);
    uint8_t*   strings  = AllocatePool(strtab.sh_size);
    bool_t     foundVMA = FALSE;

    e_handle->SetPosition(e_handle, symtab.sh_offset); 
    e_handle->Read(e_handle, &symtab.sh_size, symbols);
    e_handle->SetPosition(e_handle, strtab.sh_offset); 
    e_handle->Read(e_handle, &strtab.sh_size, strings);

    for(uint64_t sym = 0; !foundVMA && sym < symtab.sh_size / symtab.sh_entsize; ++sym)
    {
        if(memcmp((uint8_t*)"__kernel_start", strings + symbols[sym].st_name, 13)) {
            foundVMA = TRUE;
            *load_address = symbols[sym].st_value;
        }
    }
    FreePool(symbols);
    FreePool(strings);


    // put each PT_LOAD section in their respective offset.
    for (Elf64_Phdr* h = phdrtable; (Elf64_Addr)h < phdrt_end; ++h)
    {
        if(h->p_type == PT_LOAD)
        {
            physoff = ptload + h->p_paddr;                             // physoff = address where phdr needs to be in memory
            e_handle->SetPosition(e_handle, h->p_offset);              // goto the offset of the phdr in the elf file
            tmp     = h->p_memsz;                                      // get size of the current phdr
            e_handle->Read(e_handle, &tmp, (void*)physoff);            // load the phdr into the memory region allocated.
            // SetMem((void*)(physoff + tmp), h->p_memsz - tmp, 0x00);    // if(h->p_memsz > h->p_filesz) we have a .bss section that needs to be initialized (0)
            Print(L"    PT_LOAD section loaded at %X (%X Bytes). offset: %X\n", physoff, tmp, ptload);
            // ondebug(
                Print(L"    PT_LOAD Section Info: \n        Offset: %lu\n        Virtual Address: %lx    Physical Address: %lx\n        File Size: %X (Bytes)\n        Memory Size: %X (Bytes)\n        Alignment: %u Bytes\n        pages allocated at: %lx\n", 
                    h->p_offset,
                    h->p_vaddr,
                    h->p_paddr,
                    h->p_filesz,
                    h->p_memsz,
                    h->p_align, 
                    physoff
                );
            // )        
       }
    }

    Print(L"    Program headers loaded Successfully,\nEntry is At (Virtual Address) %lx\n", ehdr.e_entry);
    FreePool(phdrtable);
    return (void*)ehdr.e_entry;
}




uint64_t get_core_count()
{
    // the CPU Cores need Individual Structs, such as stacks.
    // We'll find the Core count and allocate accordingly.
    EFI_STATUS                       status  = EFI_SUCCESS;
    EFI_GUID                         mpsvsg  = EFI_MP_SERVICES_PROTOCOL_GUID;
    Efi_mp_services*                 mpsvs   = NULL;
    uint64_t                         cores   = 0;
    uint64_t                         enabled = 0;

    Print(L"Finding Core Count through MP Services...\n");
    
    status = uefi_call_wrapper(gBS->LocateProtocol, 3, &mpsvsg, NULL, (void**)&mpsvs);
    if(status) {
        Print(L"   Couldn't find Core count! - MP Services aren't available\n");
        return 0xFAFAFAFAFAFAFAFA;
    }

    status = uefi_call_wrapper(mpsvs->GetNumberOfProcessors, 3, mpsvs, &cores, &enabled);
    if(status) {
        Print(L"   Couldn't find Core Count! - function call FAILED (GetNumberOfProcessors())");
        return 0xFAFAFAFAFAFAFAFA;
    }


    Print(L"    Processor Core-Count is %u\n", cores);
    return cores;
}



void get_verify_acpi_rsdp(OUT RSDPdescriptor20** dsc)
{
    EFI_STATUS status      = EFI_SUCCESS;
    EFI_GUID   acpi20_guid = ACPI_20_TABLE_GUID;
    EFI_GUID   acpi10_guid = ACPI_TABLE_GUID;
    uint8_t*   tmp         = 0;
    uint8_t    checksum    = 0;

    Print(L"\nProbing ACPI Tables...\n");
    status = LibGetSystemConfigurationTable(&acpi20_guid, (void**)&tmp);
    if(status == EFI_NOT_FOUND) {
        Print(L"    Couldn't find ACPI_2.0+, Using ACPI_1.0 instead.\n");
        status = LibGetSystemConfigurationTable(&acpi10_guid, (void**)&tmp);

        CHECKERR(status, Print(L"   Couldn't find ACPI_1.0 ==> This System doesn't support ACPI"))
    }
    
    Print(L"    Found RSDP Descriptor at address %X in memory\n    ", tmp);
    for(uint8_t i = 0; i < (uint8_t)sizeof(RSDPdescriptor20); ++i)
    {
        checksum += tmp[i];
        ondebug( Print(L"%x ", tmp[i]); )
    }
    CHECKERR(checksum, Print(L"    RSDP descriptor Checksum Failed, Table is invalid. Sum: %u\n", checksum))
    Print(L"\n    RSDP Descriptor for ACPI_v%u Table Checksum Verified Successfully\n", ((RSDPdescriptor20*)tmp)->first.revision);

    *dsc = (RSDPdescriptor20*)tmp;
    return;
}

