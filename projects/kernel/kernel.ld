OUTPUT_FORMAT(elf64-x86-64)
ENTRY(_start)

__kernel_STACK_SIZE     = 64K;
__kernel_start          = 0;
__kernel_vma            = 0xffffff8000000000; /* 0xFFFF000000000000 = 0 TiB. We're linking at the last 512GiB of the address space. */
__trampoline_size_bytes = 0;
__kernel_vmoffset       = 0;


SECTIONS
{
    . = __kernel_vma;
 
 
    __trampoline_start = .;
    .trampoline ALIGN(4K) : AT(ADDR(.trampoline) - __trampoline_start)
    {
        build/obj/*/old.o(.text)  
        build/obj/*/old.o(.data)  
        build/obj/*/old.o(.rodata)
        build/obj/*/old.o(.bss)   
        build/obj/*/old.o(.rela)
    }
    __trampoline_end   = .;


    __trampoline_size_bytes = (
        ( (__trampoline_end - __trampoline_start) & 0xfff ) > 0
        +
        ( (__trampoline_end - __trampoline_start) / 0x1000 ) 
    ) * 0x1000;


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