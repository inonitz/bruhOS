set OSNAME=bruhOS
set CURRENT_DIR=%~dp0
set PARENT_DIR=%CURRENT_DIR%..\
set BUILDDIR=%CURRENT_DIR%bin
set OVMFDIR=%CURRENT_DIR%..\OVMFbin
set OVMFVARS=OVMF_VARS-pure-efi.fd

copy %PARENT_DIR%\ovmf_good_bin\%OVMFVARS% %PARENT_DIR%\OVMFbin\%OVMFVARS% 


qemu-system-x86_64 -cpu max,+ssse3,+sse4.1,+sse4.2,+tsc-deadline ^
	-vga vmware ^
	-monitor stdio ^
	-drive format=raw,file=%BUILDDIR%\%OSNAME%.img ^
	-m 2G ^
	-smp 12^
	-drive if=pflash,format=raw,unit=0,file=%OVMFDIR%\OVMF_CODE-pure-efi.fd,readonly=on ^
	-drive if=pflash,format=raw,unit=1,file=%OVMFDIR%\OVMF_VARS-pure-efi.fd ^
	-net none
pause

REM additional arguments to use when running QEMU: -no-hpet, 8G, -trace events=%CURRENT_DIR%/track_events.txt