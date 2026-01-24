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