cmake_minimum_required(VERSION 3.20)
project(tensor_lib VERSION 1.0.0 LANGUAGES CXX)

# Keep Release optimization consistent for this project and fetched dependencies.
add_compile_options(
    "$<$<AND:$<CONFIG:Release>,$<COMPILE_LANG_AND_ID:CXX,GNU,Clang,AppleClang,IntelLLVM>>:-O2>"
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(tensor_lib_static STATIC src/tensor.cpp)
add_library(tensor_lib_shared SHARED src/tensor.cpp)

foreach(tensor_lib_target IN ITEMS tensor_lib_static tensor_lib_shared)
    target_include_directories(${tensor_lib_target} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
    target_compile_features(${tensor_lib_target} PUBLIC cxx_std_17)
    set_target_properties(${tensor_lib_target} PROPERTIES CXX_EXTENSIONS OFF)
endforeach()

include(CTest)
if(BUILD_TESTING)
    include(FetchContent)
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.15.2
        GIT_SHALLOW TRUE
    )
    FetchContent_MakeAvailable(googletest)

    add_subdirectory(gtest)
    target_compile_features(tensor_lib_tests PRIVATE cxx_std_17)
    set_target_properties(tensor_lib_tests PROPERTIES CXX_EXTENSIONS OFF)

    add_test(NAME tensor_lib_tests COMMAND tensor_lib_tests)
endif()

option(TENSOR_LIB_BUILD_BENCHMARKS "Build tensor_lib benchmarks" ON)
if(TENSOR_LIB_BUILD_BENCHMARKS)
    include(FetchContent)
    set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "" FORCE)
    set(BENCHMARK_ENABLE_INSTALL OFF CACHE BOOL "" FORCE)
    FetchContent_Declare(
        googlebenchmark
        GIT_REPOSITORY https://github.com/google/benchmark.git
        GIT_TAG v1.9.1
        GIT_SHALLOW TRUE
    )
    FetchContent_MakeAvailable(googlebenchmark)

    add_subdirectory(benchmark)
    target_compile_features(tensor_lib_benchmark PRIVATE cxx_std_17)
    set_target_properties(tensor_lib_benchmark PROPERTIES CXX_EXTENSIONS OFF)
endif()
