cmake_minimum_required(VERSION 3.20)
project(static_library_template VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(greeting STATIC src/greeting.cpp)
add_library(Greeting::greeting ALIAS greeting)
target_include_directories(greeting PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_compile_features(greeting PUBLIC cxx_std_17)
set_target_properties(greeting PROPERTIES CXX_EXTENSIONS OFF)

add_executable(greeting_app src/main.cpp)
target_link_libraries(greeting_app PRIVATE Greeting::greeting)
set_target_properties(greeting_app PROPERTIES CXX_EXTENSIONS OFF)
