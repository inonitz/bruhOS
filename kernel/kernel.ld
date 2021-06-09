OUTPUT_FORMAT(elf64-x86-64)
ENTRY(_start)

__kernel_vma    = 0xffffff8000000000; /* 0xFFFF000000000000 = 0 TiB. We're linking at the last 512GiB of the address space. */
STACK_SIZE      = 128K;


SECTIONS
{   
    . = __kernel_vma;

	__kernel_start = .;
    .text ALIGN(4K) : AT(ADDR(.text) - __kernel_vma)
    {
        *(.text)
    }


   .data ALIGN(4K) : AT(ADDR(.data) - __kernel_vma)
   {
        *(.data)
   }


	.rodata ALIGN(4K) : AT(ADDR(.rodata) - __kernel_vma)
	{
		*(.rodata*)
	}
   
   
   .bss ALIGN(4K) : AT(ADDR(.bss) - __kernel_vma)
   {
       __kernel_bss_start = .;
       *(.bss)
        *(COMMON)
       __kernel_bss_end = .;
   }

   .stack ALIGN(4K) : AT(ADDR(.stack) - __kernel_vma)
   {
	   	__kernel_stack_bottom = .;
		. += STACK_SIZE;
		__kernel_stack_top 	  = .;
   }

   __kernel_end = .;


   /DISCARD/ :
   {
        *(.comment)
		*(.eh_frame)
   }
}