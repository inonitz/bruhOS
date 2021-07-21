# bruhOS
An x86_64 Operating System I've been developing. 
Decided to move this to a new repository because it is easier to work with.<br>
<br>
Also Learning to use Git and GitHub.
<br>
<br>
<br>
<br>
Builds Succeed In: QEMU<br>
Builds Fail    In: VirtualBox, Bare Metal (Bootloader Issues)<br>
<br>
Currently Implemented Features:<br>
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
<br>
TODO:<br>
  16. Syscalls<br>
  17. Context Switches<br>
  18. ext2 File System<br>
  19. USB 3.0 Driver Support<br>
  20. PCIe Driver Support<br>
  21. USB Keyboard & Mouse<br>
  22. Userspace<br>
  23. Scheduler (MLFQ, Not finished yet...)<br>
  24. ???<br>
