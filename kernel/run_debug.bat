set OSNAME=bruhOS
set CURRENT_DIR=%~dp0
set PARENT_DIR=%CURRENT_DIR%..\
set BUILDDIR=%CURRENT_DIR%bin
set OVMFDIR=%CURRENT_DIR%..\OVMFbin
set OVMFVARS=OVMF_VARS-pure-efi.fd

copy %PARENT_DIR%\ovmf_good_bin\%OVMFVARS% %PARENT_DIR%\OVMFbin\%OVMFVARS% 


qemu-system-x86_64 -cpu max,+tsc-deadline^
		-no-shutdown ^
		-no-reboot ^
		-vga vmware ^
		-monitor stdio ^
		-drive format=raw,file=%BUILDDIR%\%OSNAME%.img ^
		-serial file:CON ^
		-m 2G ^
		-smp 12 ^
		-drive if=pflash,format=raw,unit=0,file=%OVMFDIR%\OVMF_CODE-pure-efi.fd,readonly=on ^
		-drive if=pflash,format=raw,unit=1,file=%OVMFDIR%\OVMF_VARS-pure-efi.fd ^
		-net none
pause


// Icelake-Server-v5
// -cpu max,+tsc-deadline
// -accel hax doesn't work for some reason :( [After downloading and setting it up of course...]
// Skylake-Client-v3,+tsc-deadline
// -cpu max,tsc-deadline=on 
// -gdb tcp:127.0.0.1:1234 -S
// -D %CURRENT_DIR%/errlog.dmp -d int/exec/cpu_reset/mmu
// -serial file:%CURRENT_DIR%/serial.log
// Log items (comma separated):
// out_asm         show generated host assembly code for each compiled TB
// in_asm          show target assembly code for each compiled TB
// op              show micro ops for each compiled TB
// op_opt          show micro ops after optimization
// op_ind          show micro ops before indirect lowering
// int             show interrupts/exceptions in short format
// exec            show trace before each executed TB (lots of logs)
// cpu             show CPU registers before entering a TB (lots of logs)
// fpu             include FPU registers in the 'cpu' logging
// mmu             log MMU-related activities
// pcall           x86 only: show protected mode far calls/returns/exceptions
// cpu_reset       show CPU state before CPU resets
// unimp           log unimplemented functionality
// guest_errors    log when the guest OS does something invalid (eg accessing a
// non-existent register)
// page            dump pages at beginning of user mode emulation
// nochain         do not chain compiled TBs so that "exec" and "cpu" show
// complete traces
// trace:PATTERN   enable trace events
// 
// Use "-d trace:help" to get a list of trace events.