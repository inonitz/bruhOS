# bruhOS
An x86_64 Operating System I've been developing. 
Decided to move this to a new repository because it is easier to work with.

Currently Implemented Features:
  1. UEFI Bootloader
  2. Graphics (Through UEFI GraphicsOutputProtocol)
  3. Higher Half Kernel (Last 512GiB of Virtual Address Space)
  4. Basic Bitmap Character Printing & printk
  5. GDT
  6. IDT
  7. TSS (Not fully implemented - needed for SMP)
  8. ACPI Support - I/O APIC, LAPIC, XSDT
  9. HPET Timer (LAPIC Timer Soon ...)
  10. Physical Memory Manager - Array of Buddy Allocators
  11. Per-Process Virtual Memory Manager/Mapper using AVL Trees
  13. Atomic Mini-Library Support using GCC Extensions
  14. SMP       (Soon...)
  15. Scheduler (MLFQ, Not finished yet...)
