#include "funcs.c"
#include <efilib.h>



// what the bootloader does:
// loads the executable sections of an elf64 file into memory,
// sets up the kernel header with the necessary information.
// enables paging in the higher half defined in the kernel linker script.
// jumps to the higher half, and executes the entry point of the kernel.
EFI_STATUS efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE* SystemTable)
{
    InitializeLib(ImageHandle, SystemTable);
    EFI_STATUS                       status   = EFI_SUCCESS;
    
    // array of handles to file system protocols
    EFI_HANDLE*                      hbuffer  = NULL;
    uint64_t                         hcount   = 0;

    // currently opened root directory stuff
    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL* filesys  = NULL;
    EFI_FILE_HANDLE                  rootdir  = NULL,
                                     kfhandle = NULL;
    uint64_t                         tmp      = 0   ;
    
    // loading the Elf64 File && Paging
    __kernel_entry                   entry;
    uint64_t                         kpstart  = 0;
    uint64_t                         kvstart  = 0;
    uint64_t                         ksize    = 0;
    uint64_t                         kstackp  = 0;
    uint64_t                         kstacksz = 0;
    
    // Kernel header & Memory Map
    kernel_header_t                  kinf;
    kernel_header_t*                 kinfHigherHalf;
    uint64_t                         mKey     = 0;
    uint32_t                         dver     = 0;

    // finding the ACPI Tables
    RSDPdescriptor20*                rsdp      = NULL;


    // for GDB debugging
    ondebug(
        EFI_LOADED_IMAGE* loaded_image = NULL;
        
        status = gBS->LocateProtocol(&gEfiLoadedImageProtocolGuid, NULL, (void**)&loaded_image);
        CHECKERR(status, Print(L"COULDN'T DEBUG EFI IMAGE!!!"));
        
        Print(L"EFI Main at %lx\nWaiting For GDB Attachment... (5 seconds)\n", loaded_image->ImageBase);
        gBS->Stall(500000); 
    );


    // disable the firmware Watchdog timer
    gBS->SetWatchdogTimer(0, 0, 0, NULL);


    // look for the kernel file in the file system.
    status = gBS->LocateHandleBuffer(ByProtocol, &gEfiSimpleFileSystemProtocolGuid, NULL, &hcount, &hbuffer);
	CHECKERR(status);
	for (uint64_t i = 0; i < hcount; ++i)
	{
		// retrieve file system Protocol for current handle
		status = gBS->OpenProtocol(
			hbuffer[i],
			&gEfiSimpleFileSystemProtocolGuid,
			(VOID**)&filesys,
			ImageHandle,
			NULL,
			EFI_OPEN_PROTOCOL_GET_PROTOCOL
		);
		CHECKERR(status);


        filesys->OpenVolume(filesys, &rootdir);                                      // open root directory
        tmp      = (uint64_t)findFile(rootdir, L"kernel.elf");                       // find the file we need.
        kfhandle = (EFI_FILE_HANDLE)(tmp * (!!tmp) + (uint64_t)kfhandle * !(!!tmp)); // if tmp == 0: keep kfhandle. else: keep tmp
        rootdir->Close(rootdir);
    }
    CHECKERR(!!!kfhandle, Print(L"Couldn't find the kernel to be loaded");)

 
    get_verify_acpi_rsdp(&rsdp);              // get the rsdt pointer, such that the kernel will be able to use acpi 
    kinf.acpi.procCount = get_core_count();   // we need to allocate structures related to SMP.
    mKey = BOOL((rsdp->first.revision == 2)); // used as tmp
    kinf.acpi.extended = mKey; 
    kinf.acpi.address  = (rsdp->xsdtAddress * mKey) + (rsdp->first.rsdtAddress* !mKey); 
    

    // load the kernel executable sections into memory, and get a pointer to the entry.
    // we also have a bunch of parameters that are initialized,
    // because we allocate all the memory for the kernel at once.
    entry = loadElf64(
        kfhandle, 
        kinf.acpi.procCount, 
        &kpstart, 
        &kvstart, 
        &ksize, 
        &kstackp, 
        &kstacksz,
        (uint64_t*)&kinf.memcfg.percpuRBP,
        (uint64_t*)&kinf.memcfg.percpuTSS,
        (uint64_t*)&kinf.gdt,
        (uint64_t*)&kinfHigherHalf
    );
    ondebug(Print(L"Output From loadElf64:\nkpstart: %lx\nkvstart: %lx\nksize: %lx\nkstackp: %lx\nkstacksz: %lx\nkinf.memcfg.percpuRBP: %lx\nkinf.memcfg.percpuTSS: %lx\nkinf.gdt: %lx\nkinfHigherHalf: %lx\n",
            kpstart, kvstart, ksize, kstackp, kstacksz, kinf.memcfg.percpuRBP, kinf.memcfg.percpuTSS, kinf.gdt, kinfHigherHalf
    ););


    initialize_screenbuffer((vec2us){0, 0}, &kinf.screen); // initialize the buffer needed to print to screen.
    kinf.memcfg.rbpAddress = kstackp + kstacksz;           // set the new rbp pointer, so that the kernel gets a new stack.
    kinf.memcfg.rbpSize    = kstacksz;                     // set the size of the rbp stack.
    kinf.efi_rt            = (void*)gRT;                   // set the runtime services pointer, so the kernel (Ex.) could shutdown the machine.


    Print(L"\n\ntoVirtual: %lx\n", kvstart - kpstart);
    Print(L"Physical Start: %lx\nVirtual Start: %lx", kpstart, kvstart);


    // get the Memory Map
    kinf.map = (efi_memory_map){ (void*)1, 0, 0, 0 };
    mKey     = 0;
    status = gBS->GetMemoryMap(&kinf.map.used_size, (EFI_MEMORY_DESCRIPTOR*)kinf.map.mmap, NULL, &kinf.map.entry_size, NULL); // this will fail at first (because size)
    kinf.map.used_size *= 2;                  // this will be modified by uefi, the map_size will remain the same.
    kinf.map.map_size   = kinf.map.used_size;
    
    status = gBS->AllocatePool(BOOT_MEMORY_MAP_TYPE, kinf.map.map_size, (void**)&kinf.map.mmap);
    SetMem(kinf.map.mmap, kinf.map.map_size, 0x00);

    status = gBS->GetMemoryMap(&kinf.map.used_size, (EFI_MEMORY_DESCRIPTOR*)kinf.map.mmap, &mKey, &kinf.map.entry_size, &dver);
    CHECKERR(status && EFI_LOAD_ERROR, Print(L"Couldn't Get The UEFI Memory Map.\n"))
    // PrintMemMapDescriptors(&kinf.map);

    status = gBS->ExitBootServices(ImageHandle, mKey);
    CHECKERR(status && EFI_LOAD_ERROR, Print(L"Couldn't Exit Boot Services and continue to kernel.\n"));

    // Map the kernel to the desired higher half address; Switch Page Directories, and jump to the kernel entry.
    enablePagingJumpToElf64Ver3(&kinf, kinfHigherHalf, entry, kpstart, kvstart);
    

    return status;
}


