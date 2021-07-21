# bruhOS
An x86_64 Operating System I've been developing. 
Decided to move this to a new repository because it is easier to work with.

Also Learning How github and git work.




Builds Succeed In: QEMU
Builds Fail    In: VirtualBox, Bare Metal (Bootloader Issues)

Currently Implemented Features:
  01. UEFI Bootloader
  02. Graphics (Through UEFI GraphicsOutputProtocol)
  03. Higher Half Kernel (Last 512GiB of Virtual Address Space)
  04. Basic Bitmap Character Printing & printk
  05. GDT
  06. IDT
  07. TSS 
  08. ACPI Support - I/O APIC, LAPIC.
  09. HPET Timer
  10. LAPIC Timer
  11. Physical Memory Manager - Array of Buddy Allocators
  12. Per-Process Virtual Memory Manager/Mapper using AVL Trees (Currently Untested, Haven't used it yet)
  13. Atomic Mini-Library using GCC Extensions
  14. SMP

TODO:
  16. Syscalls
  17. Context Switches
  18. ext2 File System
  19. USB 3.0 Driver Support
  20. PCIe Driver Support
  21. USB Keyboard & Mouse
  22. Userspace
  23. Scheduler (MLFQ, Not finished yet...)
  24. ???
