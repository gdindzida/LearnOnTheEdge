cmake_minimum_required(VERSION 3.20)
project(benchmark_template VERSION 1.0.0 LANGUAGES CXX)

add_compile_options(
    "$<$<AND:$<CONFIG:Release>,$<COMPILE_LANG_AND_ID:CXX,GNU,Clang,AppleClang,IntelLLVM>>:-O2>"
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)
# Build the benchmark library without its own tests or installation rules.
set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "" FORCE)
set(BENCHMARK_ENABLE_INSTALL OFF CACHE BOOL "" FORCE)
FetchContent_Declare(
    googlebenchmark
    GIT_REPOSITORY https://github.com/google/benchmark.git
    GIT_TAG v1.9.1
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(googlebenchmark)

add_executable(sum_benchmark src/sum_benchmark.cpp)
target_compile_features(sum_benchmark PRIVATE cxx_std_17)
target_link_libraries(sum_benchmark PRIVATE benchmark::benchmark_main)
set_target_properties(sum_benchmark PROPERTIES CXX_EXTENSIONS OFF)
