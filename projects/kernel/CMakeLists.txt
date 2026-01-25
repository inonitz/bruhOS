cmake_minimum_required(VERSION 3.8..4.0)
include(GenerateLinkerScript.cmake)


project(kernel
    VERSION 0.0.1
    DESCRIPTION "x86_64 Monolithic Kernel Written in C & Intel-NASM Syntax"
    LANGUAGES ASM_NASM C
)


# The issue is that Windows-Hosted Clang defaults to gcc for linking on x86_64-unknown-none-elf targets
# More Specifically, the clang Driver redirects to gcc. 
# ld.lld can link the files perfectly fine, we do not need gcc for this.
# Unfortunately, the only good way I found for now (-fuse-ld=... doesn't work) 
# is to override the linker rule & executable for this specific project
# See:
# https://forum.osdev.org/viewtopic.php?t=56486
#
set(CMAKE_LINKER
    ${CMAKE_KERNEL_LINKER}
)
set(CMAKE_C_LINK_EXECUTABLE
    "<CMAKE_LINKER> <FLAGS> <CMAKE_C_LINK_FLAGS> <LINK_FLAGS> <OBJECTS> -o <TARGET> <LINK_LIBRARIES>"
)


set(APCORE_TRAMPOLINE_TARGET 
    ap_startup_obj
)

set(KERNEL_LINKER_SCRIPT_GENERATED_FULLNAME
    ""
)




set(KERNEL_CFLAGS 
    $<$<COMPILE_LANGUAGE:C>:-target x86_64-unknown-none-elf>
    # $<$<COMPILE_LANGUAGE:C>:-fPIC>
    $<$<COMPILE_LANGUAGE:C>:-ffreestanding>
    $<$<COMPILE_LANGUAGE:C>:-mcmodel=large>
    $<$<COMPILE_LANGUAGE:C>:-nostdlib>
    $<$<COMPILE_LANGUAGE:C>:-Os>
    $<$<AND:$<COMPILE_LANGUAGE:C>,$<CONFIG:Debug>>:-g>
    $<$<COMPILE_LANGUAGE:C>:-fno-exceptions>
    $<$<COMPILE_LANGUAGE:C>:-fshort-wchar>
    $<$<COMPILE_LANGUAGE:C>:-mno-red-zone>
    $<$<COMPILE_LANGUAGE:C>:-mgeneral-regs-only>
    $<$<COMPILE_LANGUAGE:C>:-Wno-missing-braces>
    $<$<COMPILE_LANGUAGE:C>:-Wno-unused-variable>
    $<$<COMPILE_LANGUAGE:C>:-Wno-unused-function>
    $<$<COMPILE_LANGUAGE:C>:-Wno-unused-but-set-variable>
    $<$<COMPILE_LANGUAGE:C>:-Wno-reserved-macro-identifier>
    $<$<COMPILE_LANGUAGE:C>:-Wno-reserved-identifier>
    $<$<COMPILE_LANGUAGE:C>:-Wno-declaration-after-statement>
    $<$<COMPILE_LANGUAGE:C>:-Wno-padded-bitfield>
    $<$<COMPILE_LANGUAGE:C>:-Wno-padded>
    $<$<COMPILE_LANGUAGE:C>:-Wno-packed>
    $<$<COMPILE_LANGUAGE:C>:-Wno-packed>
)


# These are flags that the clang driver understands, not ld.lld
# set(KERNEL_LINKFLAGS 
#     -target x86_64-unknown-none-elf
#     -ffreestanding
#     -mcmodel=large
#     -fPIC
#     -static
#     -nostdlib
#     -nostdinc
#     -nostartfiles
#     --gcc-toolchain=""
#     -Wl,-T,${KERNEL_LD_SCRIPT_FULLNAME}
#     -Wl,--no-gcc-toolchain
#     -Wl,--no-standard-libraries
#     -v
# )
set(KERNEL_LINKFLAGS 
    # --pie
    --static
    --nostdlib
    --no-dependent-libraries
    --verbose
)


set(SOURCES
    source/acpi/init.c
    source/acpi/ioapic.c
    source/acpi/lapic.c
    source/acpi/pic.c
    source/acpi/picdisable.s
    source/device/hpet.c
    source/device/lapict.c
    source/entry/entry.c
    source/entry/kernel.c
    source/gpcs2/blit.c
    source/gpcs2/console
    source/gpcs2/console/console.c
    source/gpcs2/console/user_ifce.c
    source/gpcs2/fonts
    source/gpcs2/fonts/Consolas12_bmp.c
    source/gpcs2/init.c
    source/gpcs2/window.c
    source/interrupts/gdt.c
    source/interrupts/handlers.c
    source/interrupts/htable.c
    source/interrupts/idt.c
    source/interrupts/init.c
    source/interrupts/inthandlers.s
    source/interrupts/load_gdt.s
    source/interrupts/router.c
    source/mem/map_ptr.c
    source/mem/phys/buddy.c
    source/mem/phys/buddyLow.c
    source/mem/phys/pmm.c
    source/mem/virt/avl
    source/mem/virt/avl/avl_tree.c
    source/mem/virt/paging.c
    source/mem/virt/vas.c
    source/mem/virt/vas_node_alloc.c
    source/scheduler/context_switch.s
    source/smp/smp.c
    source/std/bit.c
    source/std/error.c
    source/std/halt.c
    source/std/io.c
    source/std/math.c
    source/std/panic.c
    source/std/pause.c
    source/std/string.c
)


set(HEADERS
    include/acpi/acpi.h
    include/acpi/init.h
    include/acpi/ioapic.h
    include/acpi/lapic.h
    include/acpi/pic.h
    include/device/cpu.h
    include/device/hpet.h
    include/device/lapict.h
    include/device/msr.h
    include/device/port.h
    include/device/ps2.h
    include/ds/finite_stack.h
    include/ds/linked_page_buffer.h
    include/entry/entry.h
    include/gpcs2/blit.h
    include/gpcs2/colours.h
    include/gpcs2/console
    include/gpcs2/console/console.h
    include/gpcs2/console/console_instance.h
    include/gpcs2/console/user_ifce.h
    include/gpcs2/enable_types.h
    include/gpcs2/fonts
    include/gpcs2/fonts/Consolas12_bmp.h
    include/gpcs2/graphics.h
    include/gpcs2/init.h
    include/gpcs2/types.h
    include/gpcs2/window.h
    include/interrupts/gdt.h
    include/interrupts/gdtsegment.h
    include/interrupts/handlers.h
    include/interrupts/htable.h
    include/interrupts/idt.h
    include/interrupts/init.h
    include/interrupts/router.h
    include/interrupts/tss.h
    include/mem/map_ptr.h
    include/mem/phys/buddy_ext.h
    include/mem/phys/buddy.h
    include/mem/phys/buddyLow.h
    include/mem/phys/pmm.h
    include/mem/virt/avl
    include/mem/virt/avl/avl_tree.h
    include/mem/virt/avl/avl_wrap.h
    include/mem/virt/flags.h
    include/mem/virt/paging.h
    include/mem/virt/types2.h
    include/mem/virt/types.h
    include/mem/virt/vas.h
    include/mem/virt/vas_node_alloc.h
    include/pcie/init.h
    include/pcie/pcie.h
    include/preboot/acpi.h
    include/preboot/framebuffer.h
    include/preboot/kheader.h
    include/preboot/linker.h
    include/preboot/memconfig.h
    include/preboot/mem_macro.h
    include/preboot/memmap.h
    include/preboot/uefi.h
    include/scheduler/process.h
    include/scheduler/schedule.h
    include/scheduler/syscall.h
    include/scheduler/thread.h
    include/smp/percpu.h
    include/smp/smp.h
    include/smp/symbols.h
    include/std/argprefix.h
    include/std/atomic.h
    include/std/bit.h
    include/std/bool.h
    include/std/compiler_warning.h
    include/std/enable_bool.h
    include/std/error.h
    include/std/error_macro.h
    include/std/halt.h
    include/std/int.h
    include/std/io.h
    include/std/lock.h
    include/std/macro2.h
    include/std/macro.h
    include/std/math.h
    include/std/panic.h
    include/std/pause.h
    include/std/static_assert.h
    include/std/stdint.h
    include/std/string.h
    include/std/tags.h
)



# The AP-Core Trampoline Target Definition
add_library(${APCORE_TRAMPOLINE_TARGET} OBJECT 
    source/smp/smp.asm
)


generate_linker_script_pre_link_time(KERNEL_LINKER_SCRIPT_GENERATED_FULLNAME ${APCORE_TRAMPOLINE_TARGET})



# The Kernel Target definition
add_executable(${PROJECT_NAME})
target_include_directories(${PROJECT_NAME} PUBLIC 
    $<$<COMPILE_LANGUAGE:C>:${CMAKE_CURRENT_SOURCE_DIR}/include>
)
target_compile_features(${PROJECT_NAME} PUBLIC c_std_11)
target_compile_options(${PROJECT_NAME} PRIVATE ${KERNEL_CFLAGS})
target_link_options(${PROJECT_NAME} PRIVATE ${KERNEL_LINKFLAGS})
target_link_options(${PROJECT_NAME} PRIVATE 
    --script=${KERNEL_LINKER_SCRIPT_GENERATED_FULLNAME}
)
target_link_libraries(${PROJECT_NAME} PRIVATE ${APCORE_TRAMPOLINE_TARGET})

target_sources(${PROJECT_NAME} PRIVATE ${SOURCES})
target_sources(${PROJECT_NAME} PRIVATE ${HEADERS})

set_target_properties(${PROJECT_NAME} PROPERTIES 
    LINKER                    ${CMAKE_KERNEL_LINKER}
    C_VISIBILITY_PRESET       hidden
    CXX_VISIBILITY_PRESET     hidden
    VISIBILITY_INLINES_HIDDEN ON
    SUFFIX                    ".elf"
    LINK_DEPENDS              ${KERNEL_LINKER_SCRIPT_GENERATED_FULLNAME}
)


# Kernel Headers INTERFACE library, for the bootloader to interface with some required kernel structs
add_library(${PROJECT_NAME}_headers INTERFACE)
add_library(KERNEL::kernel_headers ALIAS ${PROJECT_NAME}_headers)
target_include_directories(${PROJECT_NAME}_headers INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/include")