set OSNAME=bruhOS
set BUILDDIR=%0/../bin
set OVMFDIR=%0/../../OVMFbin
set OVMFVARS=OVMF_VARS-pure-efi.fd

set CURRENT_DIR=%~dp0
set PARENT_DIR=%CURRENT_DIR%\..\
copy %PARENT_DIR%\ovmf_good_bin\%OVMFVARS% %PARENT_DIR%\OVMFbin\%OVMFVARS% 


qemu-system-x86_64 -s -D %CURRENT_DIR%/errlog.dmp -d cpu_reset -drive file=%BUILDDIR%/%OSNAME%.img -m 512M -cpu qemu64 -drive if=pflash,format=raw,unit=0,file="%OVMFDIR%/OVMF_CODE-pure-efi.fd",readonly=on -drive if=pflash,format=raw,unit=1,file="%OVMFDIR%/OVMF_VARS-pure-efi.fd" -net none
pause


// -D %CURRENT_DIR%/errlog.dmp -d int