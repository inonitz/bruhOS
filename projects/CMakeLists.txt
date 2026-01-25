cmake_minimum_required(VERSION 3.8..4.0)
# include(GenerateVsCodeLaunchJson.cmake)


project(ProjectArtifactBuilder
    VERSION 0.0.1
    DESCRIPTION "
        Defines variables, environment, and produces artifacts according to 
        the Bootloader & Kernel projects
    "
)


add_subdirectory(kernel)
add_subdirectory(bootloader)


set(TARGET_FILENAME
    primOSImage
)

set(ISO_IMAGE_NAME
    ${TARGET_FILENAME}.img
)
set(ISO_IMAGE_PATH
    "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${TARGET_FILENAME}.img"
)
set(ISO_IMAGE_BOOTLOADER_PATH
    "$<TARGET_FILE:bootloader>"
)
set(ISO_IMAGE_KERNEL_PATH
    "$<TARGET_FILE:kernel>"
)


set(ISO_IMAGE_AUTORUN_STARTUP_NSH 
    ${BOOTLOADER_STARTUP_NSH_FULLPATH}
) 

message(STATUS "AAAAAAAAAAAA 
    ${BOOTLOADER_OVMF_EFI_CODE}
    ${BOOTLOADER_OVMF_EFI_VARS}
    ${BOOTLOADER_STARTUP_NSH_FULLPATH}\n
    ${ISO_IMAGE_PATH}\n
    ${ISO_IMAGE_BOOTLOADER_PATH}\n
    ${ISO_IMAGE_KERNEL_PATH}\n
    ${ISO_IMAGE_AUTORUN_STARTUP_NSH}
")


if(CMAKE_UNDERLYING_OS_PLATFORM STREQUAL "Linux")
    add_custom_command(
        OUTPUT ${ISO_IMAGE_PATH}
        COMMAND mkdir -p img
        COMMAND cd img
        COMMAND dd if=/dev/zero of=${ISO_IMAGE_NAME} bs=1M count=64
        COMMAND mformat -F -i "${ISO_IMAGE_NAME}" ::
        COMMAND mmd -i "${ISO_IMAGE_NAME}" ::/EFI
        COMMAND mmd -i "${ISO_IMAGE_NAME}" ::/EFI/BOOT
        COMMAND mcopy -v -i ${ISO_IMAGE_NAME} "${ISO_IMAGE_BOOTLOADER_PATH}" ::/EFI/BOOT/bootx64.efi
        COMMAND mcopy -v -i ${ISO_IMAGE_NAME} "${ISO_IMAGE_AUTORUN_STARTUP_NSH}" ::
        COMMAND mcopy -v -i ${ISO_IMAGE_NAME} "${ISO_IMAGE_KERNEL_PATH}" ::
        COMMAND cp ${ISO_IMAGE_NAME} ${ISO_IMAGE_PATH}
        COMMAND echo Copied ${ISO_IMAGE_NAME} to ${ISO_IMAGE_PATH}
        COMMAND cd ../

        # 3. Dependencies: This ensures the image rebuilds if the binaries change
        DEPENDS kernel bootloader "${ISO_IMAGE_AUTORUN_STARTUP_NSH}"
        COMMENT "Generating bootable EFI kernel image"
        VERBATIM
    )
elseif(CMAKE_UNDERLYING_OS_PLATFORM STREQUAL "Windows")
    # There is a problem here...
    # Because mtools & dd can only be Acquired through mingw / msys2 / cygwin,
    # You need to install either *nix environment manager for everything to work (along with LLVM-Clang):
    # Specifically for msys2 (ucrt64):
    #   pacman -S mingw-w64-ucrt-x86_64-mtools
    # Add the following Installation Folders to your PATH:
    #   $MSYS_INSTALL_FOLDER\\usr\\bin
    #   $MSYS_INSTALL_FOLDER\\ucrt64\\bin
    # usr/bin    - dd
    # ucrt64/bin - mtools


    # Sources for mkdir command:
    # https://notepad.onghu.com/2022/windows-cmd-send-to-dev-null/
    # https://stackoverflow.com/questions/905226/what-is-equivalent-to-linux-mkdir-p-in-windows
    # https://stackoverflow.com/questions/1164049/batch-files-error-handling
    add_custom_command(
        OUTPUT ${ISO_IMAGE_PATH}
        COMMAND mkdir img > NUL 2>&1 || echo ""
        COMMAND cd img
        COMMAND dd if=/dev/zero of=${ISO_IMAGE_NAME} bs=1M count=64
        COMMAND mformat -F -i "${ISO_IMAGE_NAME}" ::
        COMMAND mmd -i "${ISO_IMAGE_NAME}" ::/EFI
        COMMAND mmd -i "${ISO_IMAGE_NAME}" ::/EFI/BOOT
        COMMAND mcopy -v -i ${ISO_IMAGE_NAME} "${ISO_IMAGE_BOOTLOADER_PATH}" ::/EFI/BOOT/bootx64.efi
        COMMAND mcopy -v -i ${ISO_IMAGE_NAME} "${ISO_IMAGE_AUTORUN_STARTUP_NSH}" ::
        COMMAND mcopy -v -i ${ISO_IMAGE_NAME} "${ISO_IMAGE_KERNEL_PATH}" ::
        COMMAND cp ${ISO_IMAGE_NAME} ${ISO_IMAGE_PATH}
        COMMAND echo Copied ${ISO_IMAGE_NAME} to ${ISO_IMAGE_PATH}
        COMMAND cd ../

        # 3. Dependencies: This ensures the image rebuilds if the binaries change
        DEPENDS kernel bootloader "${ISO_IMAGE_AUTORUN_STARTUP_NSH}"
        COMMENT "Generating bootable EFI kernel image"
        VERBATIM
    )
endif()



# OVMF Binaries exposed through dependencies/CMakeLists.txt
set(QEMU_FLAGS
    ${QEMU_DEBUG_FLAGS}
    -cpu max,+tsc-deadline
	-vga vmware
	-monitor stdio
    -drive file="${ISO_IMAGE_PATH}"
	-m 1G
	-smp 6
	-net none
	-drive if=pflash,format=raw,unit=0,file="${BOOTLOADER_OVMF_EFI_CODE}",readonly=on
	-drive if=pflash,format=raw,unit=1,file="${BOOTLOADER_OVMF_EFI_VARS}"
    -serial file:qemudebug.txt
)



add_custom_target(${TARGET_FILENAME} ALL DEPENDS ${ISO_IMAGE_PATH})
add_custom_target(run_${TARGET_FILENAME}
    COMMAND qemu-system-x86_64 ${QEMU_FLAGS}
    DEPENDS ${ISO_IMAGE_PATH}
    USES_TERMINAL
)


add_custom_target(debug_${TARGET_FILENAME}
    COMMAND qemu-system-x86_64 -s -S ${QEMU_FLAGS}
    DEPENDS ${ISO_IMAGE_PATH}
    USES_TERMINAL
)