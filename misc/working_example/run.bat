qemu-system-x86_64 -cpu max,+ssse3,+sse4.1,+sse4.2,+tsc-deadline ^
    -vga vmware ^
    -monitor stdio ^
    -drive format=raw,file="bin\Debug_amd64_kernel\kernel.img" ^
    -m 2G ^
    -smp 12^
    -drive if=pflash,format=raw,unit=0,file="ovmf\CODE_EFI.fd",readonly=on ^
    -drive if=pflash,format=raw,unit=1,file="ovmf\VARS_EFI.fd" ^
    -net none


pause
