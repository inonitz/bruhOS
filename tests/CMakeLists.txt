cmake_minimum_required(VERSION 3.14)
project(test 
    VERSION 0.0.1
    DESCRIPTION "C/C++ Programs to Test Functionality of the bootloader/kernel"
    LANGUAGES C CXX
)


set(SOURCES
    source/bootloader/gtest_print.c
    source/bootloader/print.c
    source/bootloader/test_buffers.c
    source/kernel/halt.s
    source/kernel/main.cpp
    source/kernel/test_buddy.c
    source/kernel/memcpy_blocksize.cpp
    source/kernel/buddyalloc_retrieve.cpp
)


add_executable(${PROJECT_NAME} ${SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
# target_link_libraries(${PROJECT_NAME} PRIVATE UTIL2::util2)


# The minimum C/C++ versions required for the library
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)
target_compile_features(${PROJECT_NAME} PUBLIC c_std_11)


add_custom_target(run_binary
    COMMAND ./${PROJECT_NAME}
    DEPENDS ${PROJECT_NAME}
    WORKING_DIRECTORY ${CMAKE_CUSTOM_BUILD_OUTPUT_DIRECTORY}
)
