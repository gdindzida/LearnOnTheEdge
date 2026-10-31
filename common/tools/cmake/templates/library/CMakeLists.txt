cmake_minimum_required(VERSION 3.20)
project(library_template VERSION 1.0.0 LANGUAGES CXX)

add_compile_options(
    "$<$<AND:$<CONFIG:Release>,$<COMPILE_LANG_AND_ID:CXX,GNU,Clang,AppleClang,IntelLLVM>>:-O2>"
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(greeting_static STATIC src/greeting.cpp)
add_library(greeting_shared SHARED src/greeting.cpp)

foreach(greeting_target IN ITEMS greeting_static greeting_shared)
    target_include_directories(${greeting_target} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
    target_compile_features(${greeting_target} PUBLIC cxx_std_17)
    set_target_properties(${greeting_target} PROPERTIES CXX_EXTENSIONS OFF)
endforeach()

add_executable(greeting_static_app src/main.cpp)
target_link_libraries(greeting_static_app PRIVATE greeting_static)
set_target_properties(greeting_static_app PROPERTIES CXX_EXTENSIONS OFF)

add_executable(greeting_shared_app src/main.cpp)
target_link_libraries(greeting_shared_app PRIVATE greeting_shared)
set_target_properties(greeting_shared_app PROPERTIES CXX_EXTENSIONS OFF)
