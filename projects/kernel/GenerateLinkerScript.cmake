function(generate_linker_script_pre_link_time full_linker_script_path object_library)
    set(script_full_path
        ${CMAKE_CURRENT_BINARY_DIR}/kernel-$<CONFIG>.ld
    )

    # Couple Of Notes (Explaining the Generator-Expression @.trampoline):
    #   1. I use TARGET_OBJECTS to get the abspath of the object file that was produced
    #   2. I then take that path relative to the linkers' CWD, s.t it'll find it (ld.lld doesn't like abspaths)
    #   3. The resulting Generator-Expression relative-Path contains a few hiccups that deter ld.lld from recognizing where the file is (e.g. "/./")
    #   4. Therefore, I use NORMAL_PATH to normalize the relative path I got, which'll be completely ready for ld.lld to use
    file(GENERATE
        OUTPUT ${script_full_path}
        CONTENT
        "
OUTPUT_FORMAT(elf64-x86-64)
ENTRY(_start)

__kernel_STACK_SIZE     = 64K;
__kernel_start          = 0;
__kernel_vma            = 0xffffff8000000000; /* 0xFFFF000000000000 = 0 TiB. We're linking at the last 512GiB of the address space. */
__trampoline_size_bytes = 0;
__kernel_vmoffset       = 0;


SECTIONS
{
    __trampoline_start = .;
    .trampoline ALIGN(4K) : AT(0x0)
    {
        $<PATH:NORMAL_PATH,$<PATH:RELATIVE_PATH,$<TARGET_OBJECTS:${object_library}>,${CMAKE_BINARY_DIR}>>(*)
    }
    __trampoline_end   = .;


    __trampoline_size_bytes = (
        ( (__trampoline_end - __trampoline_start) & 0xfff ) > 0
        +
        ( (__trampoline_end - __trampoline_start) / 0x1000 ) 
    ) * 0x1000;


    . = __kernel_vma;
    __kernel_start = __kernel_vma - __trampoline_size_bytes;


    .text ALIGN(4K) : AT(ADDR(.text) - __kernel_start)
    {
        *(.text)
    }


    .data ALIGN(4K) : AT(ADDR(.data) - __kernel_start)
    {
        *(.data)
    }


	.rodata ALIGN(4K) : AT(ADDR(.rodata) - __kernel_start)
	{
		*(.rodata*)
	}


    .bss ALIGN(4K) : AT(ADDR(.bss) - __kernel_start)
    {
        __kernel_bss_start = .;
       *(.bss)
        *(COMMON)
        __kernel_bss_end = .;
    }

    .stack ALIGN(4K) : AT(ADDR(.stack) - __kernel_start)
    {
        __kernel_stack_bottom = .;
		. += __kernel_STACK_SIZE;
		__kernel_stack_top 	  = .;
    }

    
    __kernel_end = .;


    /DISCARD/ :
    {
        *(.comment)
		*(.eh_frame)
        *(.note.gnu.build-id)
    }
}
        "
    )

    set(${full_linker_script_path} ${script_full_path} PARENT_SCOPE)
endfunction()