cmake_minimum_required(VERSION 3.20)
project(tensor_lib VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(tensor_lib_static STATIC src/tensor.cpp)
add_library(tensor_lib_shared SHARED src/tensor.cpp)

foreach(tensor_lib_target IN ITEMS tensor_lib_static tensor_lib_shared)
    target_include_directories(${tensor_lib_target} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
    target_compile_features(${tensor_lib_target} PUBLIC cxx_std_17)
    set_target_properties(${tensor_lib_target} PROPERTIES CXX_EXTENSIONS OFF)
endforeach()

add_executable(tensor_lib_static_app src/main.cpp)
target_link_libraries(tensor_lib_static_app PRIVATE tensor_lib_static)
set_target_properties(tensor_lib_static_app PROPERTIES CXX_EXTENSIONS OFF)

add_executable(tensor_lib_shared_app src/main.cpp)
target_link_libraries(tensor_lib_shared_app PRIVATE tensor_lib_shared)
set_target_properties(tensor_lib_shared_app PROPERTIES CXX_EXTENSIONS OFF)
