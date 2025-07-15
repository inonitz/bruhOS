# bruhOS
Homebrew x86_64 Operating System (Originally Forked From ***[PonchoOS](https://github.com/Absurdponcho/PonchoOS)***) 
<br> 
**Build:**
  - [Success] QEMU
  - [Fail]    VirtualBox Bare Metal (***Bootloader Issues :|***)

<br>**Currently Implemented:**
  1. UEFI Bootloader
  2. Graphics (Through UEFI GraphicsOutputProtocol)
  3. Higher Half Kernel (Last 512GiB of Virtual Address Space)
  4. Basic Bitmap Character Printing & printk
  5. GDT
  6. IDT
  7. TSS 
  8. ACPI Support - I/O APIC, LAPIC.
  09. HPET Timer
  10. LAPIC Timer
  11. Physical Memory Manager - Array of Buddy Allocators
  12. Per-Process Virtual Memory Manager/Mapper using AVL Trees (Currently Untested, Haven't used it yet)
  13. Atomic Mini-Library using GCC Extensions
  14. SMP

<br>**Roadmap/TODO:**
  1. Syscalls
  2. Context Switches
  3. ext2 File System
  4. USB 3.0 Driver Support
  5. PCIe Driver Support
  6. USB Keyboard & Mouse
  7. Userspace
  8. Scheduler (MLFQ, Not finished yet...)
  9. ???
