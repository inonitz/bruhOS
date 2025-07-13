#ifndef __BOOTLOADER_C_DEFINITION_LOAD_ELF_KERNEL__
#define __BOOTLOADER_C_DEFINITION_LOAD_ELF_KERNEL__
#include <bootloader/funcargtype.h>
#include <bootloader/api.h>


typedef struct __elf64_loaded_program_with_multicore_stacks
{
    uint64_t PHYSICAL startp;
    uint64_t VIRTUAL  startv;
    uint64_t          totalSize;
    uint64_t          loadSize;

    uint64_t          endOffset;

    uint64_t PHYSICAL stackSection;
    uint64_t          stackSectionSize;

    uint64_t          percpuStackOffset;
    uint64_t          percpuTSSOffset;
    uint64_t          GDTOffset;

    uint64_t VIRTUAL  postbootServicesHeader;
} loadedElf64Program;


/* 
    There are multiple stages to loading the kernel:
    1. Verifying the ELF Header
    2. Finding the ".stack" section - this is the kernels' stack in memory
    3. Finding the virtual offset of the kernel, defined in the symbol "__kernel_start"
    4. Calculating the whole size of the kernel - including Per-Cpu-Core Static Structures
    5. Finding all loadable "PT_LOAD" Program Headers, and loading them into the memory
        we allocated from step 4
    6. Populating all relevant data in "program_config" and returning the kernel entry ptr
*/
EFI_API void* loadKernel(
    IN  EFI_FILE_HANDLE     e_handle, 
    IN  uint64_t            cpu_core_count,
    OUT loadedElf64Program* program_config
);


#endif /* __BOOTLOADER_C_DEFINITION_LOAD_ELF_KERNEL__ */
