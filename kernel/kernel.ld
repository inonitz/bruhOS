OUTPUT_FORMAT(elf64-x86-64)
ENTRY(_start)

__kernel_vma     = 0xffffff8000000000; /* 0xFFFF000000000000 = 0 TiB. We're linking at the last 512GiB of the address space. */
STACK_SIZE       = 64K;
trampoline_align = 0;


SECTIONS
{
	__kernel_start = __kernel_vma;


    __trampoline_start = .;
    .trampoline ALIGN(4K) :
    {
        /* Put the Assembly inside the Trampoline Section */
        lib/smp/smp_asm.o(.text)
        lib/smp/smp_asm.o(.data)
        lib/smp/smp_asm.o(.rodata)
        lib/smp/smp_asm.o(.bss)             
        lib/smp/smp_asm.o(.rela)
    }
    __trampoline_end   = .;


    trampoline_align = ( (__trampoline_end & 0xfff) > 0 + (__trampoline_end / 0x1000) ) * 0x1000;
    __kernel_start  -= trampoline_align; 
    

    . = __kernel_vma;
    
    .text ALIGN(4K) : AT(ADDR(.text) - __kernel_vma + trampoline_align)
    {
        *(.text)
    }


   .data ALIGN(4K) : AT(ADDR(.data) - __kernel_vma + trampoline_align)
   {
        *(.data)
   }


	.rodata ALIGN(4K) : AT(ADDR(.rodata) - __kernel_vma + trampoline_align)
	{
		*(.rodata*)
	}
   
   
   .bss ALIGN(4K) : AT(ADDR(.bss) - __kernel_vma + trampoline_align)
   {
       __kernel_bss_start = .;
       *(.bss)
        *(COMMON)
       __kernel_bss_end = .;
   }

   .stack ALIGN(4K) : AT(ADDR(.stack) - __kernel_vma + trampoline_align)
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