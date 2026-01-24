@echo -off
mode 80 25
cls


for %i in fs0 fs1 fs2 fs3 fs4 fs5 fs6 fs7
  if exist %i:\efi\boot\bootx64.efi then
    %i:
    \efi\boot\bootx64.efi
    goto END
  endif
endfor

echo "Bootloader not found!"
:END