cmake_minimum_required(VERSION 3.20)
project(hexagon_blur_template VERSION 1.0.0 LANGUAGES C CXX ASM)

add_compile_options(
    "$<$<AND:$<CONFIG:Release>,$<COMPILE_LANG_AND_ID:C,GNU,Clang,AppleClang,IntelLLVM>>:-O2>"
    "$<$<AND:$<CONFIG:Release>,$<COMPILE_LANG_AND_ID:CXX,GNU,Clang,AppleClang,IntelLLVM>>:-O2>"
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# build_cmake selects a Qualcomm toolchain before project() runs. Ordinary
# cmake without a toolchain builds a CPU-only version for local development.
set(NO_QURT_INC 1)
if(CMAKE_SYSTEM_PROCESSOR STREQUAL "Hexagon" OR OS_TYPE STREQUAL "HLOS")
    if(NOT HEXAGON_SDK_ROOT)
        set(HEXAGON_SDK_ROOT "$ENV{HEXAGON_SDK_ROOT}")
    endif()
    include("${HEXAGON_SDK_ROOT}/build/cmake/hexagon_fun.cmake")
    include_directories(
        "${CMAKE_CURRENT_BINARY_DIR}"
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
        "${HEXAGON_SDK_ROOT}/incs"
        "${HEXAGON_SDK_ROOT}/incs/stddef"
    )
endif()

if(CMAKE_SYSTEM_PROCESSOR STREQUAL "Hexagon")
    # Only C is needed on the DSP: no additional DSP C++ runtime to deploy.
    add_library(image_blur_skel SHARED
        "${CMAKE_CURRENT_BINARY_DIR}/image_blur_skel.c"
        dsp/image_blur_imp.c
        common/blur_kernel.c
    )
    target_compile_features(image_blur_skel PRIVATE c_std_99)
    build_idl(inc/image_blur.idl image_blur_skel)
    copy_binaries(image_blur_skel)
else()
    add_library(blur_kernel STATIC common/blur_kernel.c)
    target_include_directories(blur_kernel PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
    target_compile_features(blur_kernel PRIVATE c_std_99)

    add_executable(image_blur_app app/main.cpp)
    target_compile_features(image_blur_app PRIVATE cxx_std_17)
    set_target_properties(image_blur_app PROPERTIES CXX_EXTENSIONS OFF)
    # SDK link_custom_library uses the plain target_link_libraries signature.
    target_link_libraries(image_blur_app blur_kernel)

    if(OS_TYPE STREQUAL "HLOS")
        if(NOT CMAKE_SYSTEM_NAME STREQUAL "UbuntuARM")
            message(FATAL_ERROR "This template's FastRPC host targets UbuntuARM Linux.")
        endif()
        add_library(image_blur SHARED "${CMAKE_CURRENT_BINARY_DIR}/image_blur_stub.c")
        build_idl(inc/image_blur.idl image_blur)
        link_custom_library(image_blur cdsprpc)
        link_custom_library(image_blur_app cdsprpc)
        target_link_libraries(image_blur_app image_blur)
        target_compile_definitions(image_blur_app PRIVATE IMAGE_BLUR_FASTRPC=1)
        # The SDK toolchain forces -std=c++11 even when GCC defaults to C++17.
        target_compile_options(image_blur_app PRIVATE -std=c++17)
        # main.cpp includes the QAIC-generated header.
        add_dependencies(image_blur_app image_blur)
        set_common_compile_and_link_options(image_blur_app)
        copy_binaries(image_blur_app image_blur)
    endif()

    if(NOT CMAKE_CROSSCOMPILING)
        include(CTest)
        if(BUILD_TESTING)
            add_executable(blur_kernel_test tests/blur_kernel_test.cpp)
            target_compile_features(blur_kernel_test PRIVATE cxx_std_17)
            target_link_libraries(blur_kernel_test PRIVATE blur_kernel)
            add_test(NAME blur_kernel COMMAND blur_kernel_test)
            add_test(NAME blur_cpu_image COMMAND image_blur_app --backend cpu --seed 42
                --output-dir "${CMAKE_CURRENT_BINARY_DIR}/test-output")
            add_test(NAME blur_invalid_backend COMMAND image_blur_app --backend gpu)
            set_tests_properties(blur_invalid_backend PROPERTIES WILL_FAIL TRUE)
            add_test(NAME blur_invalid_seed COMMAND image_blur_app --backend cpu --seed -1)
            set_tests_properties(blur_invalid_seed PROPERTIES WILL_FAIL TRUE)
            if(NOT OS_TYPE STREQUAL "HLOS")
                add_test(NAME blur_dsp_unavailable COMMAND image_blur_app --backend dsp)
                set_tests_properties(blur_dsp_unavailable PROPERTIES WILL_FAIL TRUE)
            endif()
        endif()
    endif()
endif()
