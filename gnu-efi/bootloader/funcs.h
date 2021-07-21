#ifndef __BOOTLOADER_FUNCTIONS__
#define __BOOTLOADER_FUNCTIONS__
#pragma once
#include "defs.h"



/* 
    * finds a file in the given directory
    * Directory - The directory to search
    * filename  - The name of the file to find
*/
EFI_FILE_HANDLE findFile(
    IN EFI_FILE_HANDLE Directory,
    IN CHAR16*         filename
);




/*
    * The function that performs the search inside findFile
    * The function returns the handle (if it is found) in out.
    * This function should be called inside the findFile function.
*/
void findFileInternal(
    IN  EFI_FILE_HANDLE  current_dir, 
    IN  CHAR16*          filename, 
    OUT EFI_FILE_HANDLE* out
);
/*
    * General algorithm
    * current_dir -> current directory being searched in
    * filename    -> the filename that is searched for
    * currfile    -> the opened file in current_dir
    * 
    * Open          : current_dir
    * get info about: currfile
    * if currfile is a directory: 
    *   look for filename in currfile (recursive call)
    * else
    *   compare names between currfile and filename
    *   if they match
    *       return currfile (we have found the handle for file with the name of "filename")
    *   else 
    *       keep searching in current_dir (until you get to the end of the directory)
*/



/* 
    * Prints the available video modes to the console (screen)
    * Should be used to determine which mode to set, using setVideoMode()
*/
void PrintAvailableVideoModes(
    IN EFI_GRAPHICS_OUTPUT_PROTOCOL* gop /* graphics output buffer, from gBS->LocateProtocol(&gEfiGraphicsOutputProtocolGuid, ...) */
);




/* 
    * Sets The video mode in which the screen buffer will be.
    * each mode has a certain resolution -> The last mode has *usually* the biggest resolution.
*/
void setVideoMode(
    IN EFI_GRAPHICS_OUTPUT_PROTOCOL* gop,
    IN uint16_t                      mode
);




/* 
    * Initializes the front buffer needed for the kernel to print/draw stuff
    * Finds a mode that has dimensions.<x, y> and sets to that mode.
    * By default if dimensions = { 0, 0 }, then the function will query for 1080p mode.  
    * if it doesn't find the mode, the framebuffer is set to the default mode (0, [res = 640x480])
*/
void initialize_screenbuffer(
    IN  vec2us                        dimensions,
    OUT framebuffer_t*                init
);




/* 
    * Modified and Taken from Kernel Source (kernel/mem/phys/pmm.h): Prints the EFI Memory Map.
*/
void PrintMemMapDescriptors(efi_memory_map* map);




/* 
    * This function does a lot of things, lets break it down:
    * 1. Paging:
    *       creates a PML4 Table and maps the whole address space,
    *       using on the virtual start of the kernel (that was retrieved from loadElf64)
    *       NOTE: This creates a temporary identity mapping, which needs to be removed when inside the Elf File entry point.
    * 
    * 2. Identity maps the functions' stack and paging tables.
    * 
    * 3. Switches to the new PML4.
    * 
    * 4. Jumps to the Kernel Entry Point.
    * 
    * kernel_header_t* header    - a pointer to where the kernel header resides in the previous stack. We'll supply the kernel with the info in this header.
    * kernel_header_t* hHalfHdr  - a pointer to where the kernel header that will be read by the kernel, reside.
    * __kernel_entry   entry     - the pointer to the starting address of the kernel (in physical memory)
    * uint64_t         physStart - the physical start of the Elf64 File. (Where it was loaded in memory)
    * uint64_t         virtStart - the virtual  start of the Elf64 File. (The vaddr of the first PT_LOAD Section) 
*/
void __attribute__((noreturn)) enablePagingJumpToElf64Ver3(
    kernel_header_t* header,
    kernel_header_t* hHalfHdr,
    __kernel_entry   entry, 
    uint64_t         physStart, 
    uint64_t         virtStart
);




/* 
    * Loads an elf64 file into memory 
    * EFI_FILE_HANDLE e_handle       - A file handle, where the ELF File is placed in the disk.
    * uint64_t        cpu_core_count - The amount of existing CPU cores on the system. for SMP.
    * uint64_t*       loaded_address - will be set to the physical address in which the ELF64 File was loaded.
    * uint64_t*       load_address   - will be set to the virtual  address in which the ELF64 File will be mapped to.
    * uint64_t*       loaded_size    - will be set to the ELF64 File Size (The amount of memory allocated for the File) in pages (multiples of 4KiB).
    * uint64_t*       progStack      - will be set to the address of the .stack section in the Elf64 File. will be null if doesn't exist.
    * uint64_t*       progStackSize  - will be set to the size of the .stack section in the Elf64 File. will be 0 if doesn't exist.
    * uint64_t*       apCoreStackOff - the offset in memory for per-cpu-core 32KiB Stacks. 
    * uint64_t*       apTssOff       - the offest in memory for per-cpu-core TSS's
    * uint64_t*       GdtOff         - the offest in memory for the GDT allocated for the kernel.
    *                                  because GDT_SIZE = GDT_ENTRY * (5 + amount_of_cores), its allocated dynamically
    *                                  when I'm allocating memory for the kernel.
    * uint64_t*       higherHalfKhdr - Because the kernel header exists in the bootloader stack,
    *                                  which WON'T be mapped to the higher half, we need a new location
    *                                  for the header. The header will also be allocated with the kernel.
    * returns a pointer to the entry of the ELF64 File, which'll be null if an error occured.
*/
void* loadElf64(
    IN  EFI_FILE_HANDLE e_handle, 
    IN  uint64_t        cpu_core_count,
    OUT uint64_t*       loaded_address, 
    OUT uint64_t*       load_address, 
    OUT uint64_t*       loaded_size,
    OUT uint64_t*       progStack,
    OUT uint64_t*       progStackSize,
    OUT uint64_t*       apCoreStackOff,
    OUT uint64_t*       apTssOff,
    OUT uint64_t*       GdtOff,
    OUT uint64_t*       higherHalfKhdr
);




/* 
    * Finds the ACPI_2.0+ / ACPI_1.0 RSDP structure in memory.
    * dsc - the address of a pointer to a RSDPdescriptor20 structure which will be initialized if an acpi rsdp is found.
    * 
    * the function verifies the checksum of the descriptor (if found); 
    * if the checksum fails, the bootloader will exit with EFI_UNSUPPORTED
    * else, the function will set the dsc variable and return successfully.
*/
void get_verify_acpi_rsdp(
    OUT RSDPdescriptor20** dsc
);


/* 
    * The name is self explanatory - finds the amount of CPU cores available.
    * In any case of a fail, the function returns 0xFAFAFAFAFAFAFAFA.
    * On Success, the function returns the amount of CPU cores available in the system.
*/
uint64_t get_core_count();



#endif
