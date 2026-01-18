set(CMAKE_UNDERLYING_OS_PLATFORM "")
set(TOOLCHAIN_PREFIX "")


set(CMAKE_SYSTEM_NAME      "Generic")
set(CMAKE_SYSTEM_PROCESSOR x86_64)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)




if( CMAKE_HOST_SYSTEM_NAME STREQUAL "Windows" )
    set(CMAKE_UNDERLYING_OS_PLATFORM "Windows")

elseif( CMAKE_HOST_SYSTEM_NAME STREQUAL "Linux")
    set(CMAKE_UNDERLYING_OS_PLATFORM "Linux")

elseif( CMAKE_HOST_SYSTEM STREQUAL "Darwin")
    set(CMAKE_UNDERLYING_OS_PLATFORM "MacOS")

else()
    include(CMakePrintHelpers)
    cmake_print_variables(CMAKE_HOST_SYSTEM_NAME)
    message(FATAL_ERROR "Unrecognized Host Platform\n")
    set(CMAKE_UNDERLYING_OS_PLATFORM "UNKNOWN")
endif()


if( CMAKE_UNDERLYING_OS_PLATFORM STREQUAL "Linux" )
    message(STATUS "Detected Linux Host Platform")
    # # ELF64_CROSS_GCC_PATH must be defined for this to work.
    # set(TOOLCHAIN_PREFIX x86_64-linux)
    # set(CMAKE_FIND_ROOT_PATH "$ENV{ELF64_CROSS_GCC_PATH}")
    # set(CMAKE_ASM_COMPILER $ENV{ELF64_CROSS_GCC_PATH}/bin/${TOOLCHAIN_PREFIX}-as)
    # set(CMAKE_C_COMPILER   $ENV{ELF64_CROSS_GCC_PATH}/bin/${TOOLCHAIN_PREFIX}-gcc)
    # set(CMAKE_CXX_COMPILER $ENV{ELF64_CROSS_GCC_PATH}/bin/${TOOLCHAIN_PREFIX}-g++)
    # set(CMAKE_AR           $ENV{ELF64_CROSS_GCC_PATH}/bin/${TOOLCHAIN_PREFIX}-ar)
    # set(CMAKE_OBJCOPY      $ENV{ELF64_CROSS_GCC_PATH}/bin/${TOOLCHAIN_PREFIX}-objcopy)
    # set(CMAKE_OBJDUMP      $ENV{ELF64_CROSS_GCC_PATH}/bin/${TOOLCHAIN_PREFIX}-objdump)
    # set(CMAKE_SIZE         $ENV{ELF64_CROSS_GCC_PATH}/bin/${TOOLCHAIN_PREFIX}-size)

    set(CMAKE_ASM_NASM_COMPILER               nasm)
    set(CMAKE_ASM_NASM_SOURCE_FILE_EXTENSIONS s)
    set(CMAKE_ASM_NASM_OBJECT_FORMAT          elf64)
    
    cmake_path(SET COMPILER_ROOT_PATH $ENV{LLVMInstallDir})
    set(CMAKE_FIND_ROOT_PATH ${COMPILER_ROOT_PATH})
    set(CMAKE_C_COMPILER   ${COMPILER_ROOT_PATH}/bin/clang)
    set(CMAKE_CXX_COMPILER ${COMPILER_ROOT_PATH}/bin/clang++)
    set(CMAKE_LINKER       ${COMPILER_ROOT_PATH}/bin/ld.lld)
    set(CMAKE_AR           ${COMPILER_ROOT_PATH}/bin/llvm-ar)
    set(CMAKE_OBJCOPY      ${COMPILER_ROOT_PATH}/bin/llvm-objcopy)
    set(CMAKE_OBJDUMP      ${COMPILER_ROOT_PATH}/bin/llvm-objdump)

    set(CMAKE_BOOTLOADER_LINKER "${COMPILER_ROOT_PATH}/bin/lld-link")
    set(CMAKE_KERNEL_LINKER     "${COMPILER_ROOT_PATH}/bin/ld.lld")

# Windows Requires the following Environment Variables (for Clang & NASM):
#   LLVMInstallDir    - Usually is defined on windows platforms when trying to use custom clang installations
#   NASM_INSTALL_PATH - Unless NASM is already in the PATH variables, I prefer to define it like this
elseif( CMAKE_UNDERLYING_OS_PLATFORM STREQUAL "Windows" )
    message(STATUS "Detected Windows Host Platform")


    # See: https://stackoverflow.com/questions/56420035/how-to-use-cmake-to-build-binaries-with-nasm
    cmake_path(SET CMAKE_ASM_NASM_COMPILER "$ENV{NASM_INSTALL_PATH}/nasm.exe")
    set(CMAKE_ASM_NASM_SOURCE_FILE_EXTENSIONS s)
    set(CMAKE_ASM_NASM_OBJECT_FORMAT          elf64)
    
    cmake_path(SET COMPILER_ROOT_PATH $ENV{LLVMInstallDir})
    set(CMAKE_FIND_ROOT_PATH ${COMPILER_ROOT_PATH})
    set(CMAKE_C_COMPILER   "${COMPILER_ROOT_PATH}/bin/clang.exe")
    set(CMAKE_CXX_COMPILER "${COMPILER_ROOT_PATH}/bin/clang++.exe")
    set(CMAKE_LINKER       "${COMPILER_ROOT_PATH}/bin/lld.exe")
    set(CMAKE_AR           "${COMPILER_ROOT_PATH}/bin/llvm-ar.exe")
    set(CMAKE_OBJCOPY      "${COMPILER_ROOT_PATH}/bin/llvm-objcopy.exe")
    set(CMAKE_OBJDUMP      "${COMPILER_ROOT_PATH}/bin/llvm-objdump.exe")

    set(CMAKE_BOOTLOADER_LINKER "${COMPILER_ROOT_PATH}/bin/lld-link.exe")
    set(CMAKE_KERNEL_LINKER     "${COMPILER_ROOT_PATH}/bin/ld.lld.exe")


elseif( CMAKE_UNDERLYING_OS_PLATFORM STREQUAL "MacOS" )
#   TODO


else()
    message(FATAL_ERROR "Unrecognized Host Platform.\n")
    set(CMAKE_FIND_ROOT_PATH "")
    set(CMAKE_ASM_COMPILER nasm)
    set(CMAKE_C_COMPILER   gcc)
    set(CMAKE_CXX_COMPILER g++)
endif()


