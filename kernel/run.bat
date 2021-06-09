set OSNAME=bruhOS
set BUILDDIR=%0/../bin
set OVMFDIR=%0/../../OVMFbin
set OVMFVARS=OVMF_VARS-pure-efi.fd

set CURRENT_DIR=%~dp0
set PARENT_DIR=%CURRENT_DIR%\..\
copy %PARENT_DIR%\ovmf_good_bin\%OVMFVARS% %PARENT_DIR%\OVMFbin\%OVMFVARS% 


qemu-system-x86_64 -vga vmware -monitor stdio -drive file=%BUILDDIR%/%OSNAME%.img -m 1G -cpu qemu64,+ssse3,+sse4.1,+sse4.2 -smp 4,maxcpus=4 -drive if=pflash,format=raw,unit=0,file="%OVMFDIR%/OVMF_CODE-pure-efi.fd",readonly=on -drive if=pflash,format=raw,unit=1,file="%OVMFDIR%/OVMF_VARS-pure-efi.fd" -net none
pause

REM additional arguments to use when running QEMU: -no-hpet, 8G, -trace events=%CURRENT_DIR%/track_events.txt