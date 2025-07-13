set OSNAME=kernel
set CURRENT_DIR=%~dp0
set PARENT_DIR=%CURRENT_DIR%..\
set BUILDDIR=%CURRENT_DIR%build\bin\Debug_amd64_kernel
set OVMFDIR=%CURRENT_DIR%dependencies\ovmf
set OVMFVAR=VARS_EFI.fd
set OVMFCODE=CODE_EFI.fd
set QEMU_SERIAL_FILE=%CURRENT_DIR%\qemuserial.txt
set QEMU_DEBUG_FILE=%CURRENT_DIR%\qemdebug.txt

qemu-system-x86_64 -cpu max,+tsc-deadline ^
	-drive format=raw,file="%BUILDDIR%\%OSNAME%.img" ^
	-gdb tcp:127.0.0.1:1234 -S ^
	-m 256M ^
	-drive if=pflash,format=raw,unit=0,file="%OVMFDIR%\%OVMFCODE%",readonly=on ^
	-drive if=pflash,format=raw,unit=1,file="%OVMFDIR%\%OVMFVAR%" ^
	-net none ^
	-D %QEMU_DEBUG_FILE% -d int,cpu_reset ^
	-monitor stdio ^
	-serial file:%QEMU_SERIAL_FILE% ^
	-no-shutdown ^
	-accel tcg
pause

REM additional arguments to use when running QEMU: -no-hpet, 8G, -trace events=%CURRENT_DIR%/track_events.txt


REM -chardev stdio,id=ovmf -device isa-debugcon,iobase=0x402,chardev=ovmf ^
REM		-gdb tcp:127.0.0.1:1234 -S ^
REM		-monitor stdio ^
REM		-serial stdio ^
REM		-serial chardev:char0 -mon chardev=char0 ^
REM 	-chardev stdio,id=char0,mux=on,logfile=serial.log,signal=off ^
REM 	-no-reboot ^


REM # Old Cmdline args I previously used
REM -cpu max,+tsc-deadline
REM -accel hax doesn't work for some reason :(
REM Skylake-Client-v3,+tsc-deadline
REM -cpu max,tsc-deadline=on 
REM -gdb tcp:127.0.0.1:1234 -S
REM -D %CURRENT_DIR%/errlog.dmp -d int/exec/cpu_reset/mmu
REM -serial file:%CURRENT_DIR%/serial.log
REM Log items (comma separated):
REM out_asm         show generated host assembly code for each compiled TB
REM in_asm          show target assembly code for each compiled TB
REM op              show micro ops for each compiled TB
REM op_opt          show micro ops after optimization
REM op_ind          show micro ops before indirect lowering
REM int             show interrupts/exceptions in short format
REM exec            show trace before each executed TB (lots of logs)
REM cpu             show CPU registers before entering a TB (lots of logs)
REM fpu             include FPU registers in the 'cpu' logging
REM mmu             log MMU-related activities
REM pcall           x86 only: show protected mode far calls/returns/exceptions
REM cpu_reset       show CPU state before CPU resets
REM unimp           log unimplemented functionality
REM guest_errors    log when the guest OS does something invalid (eg accessing a
REM non-existent register)
REM page            dump pages at beginning of user mode emulation
REM nochain         do not chain compiled TBs so that "exec" and "cpu" show
REM complete traces
REM trace:PATTERN   enable trace events
REM 
REM Use "-d trace:help" to get a list of trace events.

REM qemu-system-x86_64 -cpu max,+tsc-deadline^
REM 		-no-shutdown ^
REM 		-no-reboot ^
REM 		-vga vmware ^
REM 		-monitor stdio ^
REM 		-drive format=raw,file=%BUILDDIR%\%OSNAME%.img ^
REM 		-serial file:CON ^
REM 		-m 2G ^
REM 		-smp 12 ^
REM 		-drive if=pflash,format=raw,unit=0,file=%OVMFDIR%\OVMF_CODE-pure-efi.fd,readonly=on ^
REM 		-drive if=pflash,format=raw,unit=1,file=%OVMFDIR%\OVMF_VARS-pure-efi.fd ^
REM 		-net none
REM pause