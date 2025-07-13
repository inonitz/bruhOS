set OSNAME=kernel
set CURRENT_DIR=%~dp0
set PARENT_DIR=%CURRENT_DIR%..\
set BUILDDIR=%CURRENT_DIR%build\bin\Debug_amd64_kernel
set OVMFDIR=%CURRENT_DIR%dependencies\ovmf
set OVMFVAR=VARS_EFI.fd
set OVMFCODE=CODE_EFI.fd

qemu-system-x86_64 -cpu max,+tsc-deadline ^
	-vga vmware ^
	-monitor stdio ^
	-drive format=raw,file="%BUILDDIR%\%OSNAME%.img" ^
	-m 256M ^
	-smp 6^
	-drive if=pflash,format=raw,unit=0,file="%OVMFDIR%\%OVMFCODE%",readonly=on ^
	-drive if=pflash,format=raw,unit=1,file="%OVMFDIR%\%OVMFVAR%" ^
	-net none
pause

REM additional arguments to use when running QEMU: -no-hpet, 8G, -trace events=%CURRENT_DIR%/track_events.txt