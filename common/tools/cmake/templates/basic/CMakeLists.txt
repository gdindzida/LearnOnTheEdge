cmake_minimum_required(VERSION 3.20)
project(basic_template VERSION 1.0.0 LANGUAGES CXX)

add_compile_options(
    "$<$<AND:$<CONFIG:Release>,$<COMPILE_LANG_AND_ID:CXX,GNU,Clang,AppleClang,IntelLLVM>>:-O2>"
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(basic_app src/main.cpp)
target_compile_features(basic_app PRIVATE cxx_std_17)
set_target_properties(basic_app PROPERTIES CXX_EXTENSIONS OFF)
