cmake_minimum_required (VERSION 3.15..4.0)
project(
    gnu-efi
    VERSION 1.0
    DESCRIPTION "Building EFI Applications Using the GNU Toolchain"
    LANGUAGES ASM_NASM C
)


add_library(${PROJECT_NAME} INTERFACE)
add_library(GNU-EFI::gnu-efi ALIAS ${PROJECT_NAME})


target_include_directories(${PROJECT_NAME} INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/gnu-efi/inc>
    $<INSTALL_INTERFACE:gnu-efi/install>
)

# target_compile_definitions(my_headers INTERFACE

# )

target_compile_options(${PROJECT_NAME} INTERFACE
    -Wall
    -mno-red-zone
    -fno-stack-protector
    -fshort-wchar
    -mno-red-zone
)

target_link_options(${PROJECT_NAME} INTERFACE

)


target_compile_definitions(${PROJECT_NAME} INTERFACE
    # -D_M_X64
    # -D__x86_64__
    # -D__amd64__
)