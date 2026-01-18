cmake_minimum_required(VERSION 3.14..4.0)
project(bootloader
    VERSION 0.0.1
    DESCRIPTION "x86_64 Bootloader for loading ELF-x86-64 Files (using GNU-EFI)"
    LANGUAGES ASM_NASM C
)


set(SOURCES
    source/acpi.c
    source/alloc.c
    source/data.c
    source/elfutil.c
    source/err.c
    source/exitbootservices.c
    source/exitloader.c
    source/file.c
    source/loadkernel.c
    source/main.c
    source/memorymap.c
    source/pause.c
    source/print.c
    source/rtdata.c
    source/string.c
    source/video.c
)


set(HEADERS
    include/bootloader/acpi.h
    include/bootloader/alloc.h
    include/bootloader/api.h
    include/bootloader/bool_macro.h
    include/bootloader/debug_print.h
    include/bootloader/elf.h
    include/bootloader/elf_small.h
    include/bootloader/elfutil.h
    include/bootloader/err.h
    include/bootloader/exitbootservices.h
    include/bootloader/exitloader.h
    include/bootloader/file.h
    include/bootloader/funcargtype.h
    include/bootloader/lib.h
    include/bootloader/loadkernel.h
    include/bootloader/memorymap.h
    include/bootloader/pause.h
    include/bootloader/print.h
    include/bootloader/string.h
    include/bootloader/uchar.h
    include/bootloader/util.h
    include/bootloader/video.h
)


set(EFI_APP_CFLAGS
    $<$<COMPILE_LANGUAGE:C>:-v>
    $<$<COMPILE_LANGUAGE:C>:-O0>
    $<$<COMPILE_LANGUAGE:C>:-target x86_64-unknown-windows>
    # $<$<COMPILE_LANGUAGE:C>:-fPIC>
    $<$<COMPILE_LANGUAGE:C>:-ffreestanding>
    $<$<COMPILE_LANGUAGE:C>:-fshort-wchar>
    $<$<COMPILE_LANGUAGE:C>:-mno-red-zone>
)

set(EFI_APP_LINKFLAGS
    -target x86_64-unknown-windows
    # -fuse-ld=lld-link
    -fPIC
    # -nostdlib
    # -nostdinc
    # -Wl,-subsystem,efi_application
    # -Wl,-entry,efi_main
    # -Wl,-image-base,0
    # -Wl,-no-reloc
    -Wl,/nodefaultlib
    -Wl,/subsystem:efi_application
    -Wl,/entry:efi_main
    -Wl,/base:0
    -Wl,/fixed
)


set(EXECUTABLE_FULL_NAME
    ${PROJECT_NAME}.efi
)


add_executable(${EXECUTABLE_FULL_NAME})
target_include_directories(${EXECUTABLE_FULL_NAME} PUBLIC 
    $<$<COMPILE_LANGUAGE:C>:${CMAKE_CURRENT_SOURCE_DIR}/include>
)
target_compile_options(${EXECUTABLE_FULL_NAME} PRIVATE ${EFI_APP_CFLAGS})
target_link_options(${EXECUTABLE_FULL_NAME} PRIVATE ${EFI_APP_LINKFLAGS})
target_sources(${EXECUTABLE_FULL_NAME} PRIVATE ${SOURCES})
target_sources(${EXECUTABLE_FULL_NAME} PRIVATE ${HEADERS})
target_link_libraries(${EXECUTABLE_FULL_NAME} PRIVATE GNU-EFI::gnu-efi)
target_link_libraries(${EXECUTABLE_FULL_NAME} PRIVATE KERNEL::kernel_headers)

# The minimum C/C++ versions required
target_compile_features(${EXECUTABLE_FULL_NAME} PUBLIC c_std_11)
set_target_properties(${EXECUTABLE_FULL_NAME} PROPERTIES 
    LINKER                    ${CMAKE_BOOTLOADER_LINKER}
    C_VISIBILITY_PRESET       hidden
    CXX_VISIBILITY_PRESET     hidden
    VISIBILITY_INLINES_HIDDEN ON
    SUFFIX                    ""
)

