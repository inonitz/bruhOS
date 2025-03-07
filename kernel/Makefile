OSNAME = bruhOS

GNUEFI   = ../gnu-efi
OVMFDIR  = ../OVMFbin
LDS 	 = kernel.ld
CC  	 = gcc
ASMC 	 = nasm
LD  	 = ld
CFLAGS   =  -ffreestanding \
			-fshort-wchar \
			-mno-red-zone \
			-mcmodel=large \
			-pedantic \
			-Wall \
			-Werror \
			-Wno-missing-braces \
			-Wno-unused-variable \
			-Wno-unused-function \
			-Wno-unused-but-set-variable \
			-std=c11 \
			-O1 \
#			-msse4.1 \
#			-msse4.2 \
			-mgeneral-regs-only \
			-fif-conversion \
			-fif-conversion2 \

ASMFLAGS = -O0
LDFLAGS  = -T $(LDS) -static -nostdlib

SRCDIR := src
OBJDIR := lib
BUILDDIR = bin
BOOTEFI := $(GNUEFI)/x86_64/bootloader/main.efi

rwildcard=$(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))

SRC    = $(call rwildcard,$(SRCDIR),*.c) 
ASMSRC = $(call rwildcard,$(SRCDIR),*.asm)       
OBJS   = $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SRC))
OBJS  += $(patsubst $(SRCDIR)/%.asm, $(OBJDIR)/%_asm.o, $(ASMSRC))
DIRS   = $(wildcard $(SRCDIR)/*)

kernel: $(OBJS) link


$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@ echo !===== Compiling $^ ... =====!
	@ mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $^ -o $@


$(OBJDIR)/%_asm.o: $(SRCDIR)/%.asm
	@ echo !===== Compiling $^ ... =====!
	@ mkdir -p $(@D)
	$(ASMC) $(ASMFLAGS) $^ -f elf64 -o $@


link:
	@ echo !===== Linking $^ ... =====!
	$(LD) $(LDFLAGS) -o $(BUILDDIR)/kernel.elf $(OBJS)


setup:
	@mkdir $(BUILDDIR)
	@mkdir $(OBJDIR)
	@mkdir $(SRCDIR)
	

clean:
	@echo !===== Wiping $^ ... =====!
	rm -r $(OBJDIR)/*


buildimg:
	dd if=/dev/zero of=$(BUILDDIR)/$(OSNAME).img bs=512 count=93750
	mformat -i $(BUILDDIR)/$(OSNAME).img -f 1440 ::
	mmd -i $(BUILDDIR)/$(OSNAME).img ::/EFI
	mmd -i $(BUILDDIR)/$(OSNAME).img ::/EFI/BOOT
	mcopy -i $(BUILDDIR)/$(OSNAME).img $(BOOTEFI) ::/EFI/BOOT
	mcopy -i $(BUILDDIR)/$(OSNAME).img startup.nsh ::
	mcopy -i $(BUILDDIR)/$(OSNAME).img $(BUILDDIR)/kernel.elf ::


run:
	qemu-system-x86_64 -drive file=$(BUILDDIR)/$(OSNAME).img -m 256M -cpu qemu64 -drive if=pflash,format=raw,unit=0,file="$(OVMFDIR)/OVMF_CODE-pure-efi.fd",readonly=on -drive if=pflash,format=raw,unit=1,file="$(OVMFDIR)/OVMF_VARS-pure-efi.fd" -net none



#$(OBJDIR)/smp/smp_asm.o: $(SRCDIR)/smp/smp.asm
#	@ echo !===== Compiling $^ bruh bruh bruh ... =====!
#	@ mkdir -p $(@D)
#	$(ASMC) $(ASMFLAGS) $^ -f elf64 -o $@


# -ggdb - for debugging with gdb