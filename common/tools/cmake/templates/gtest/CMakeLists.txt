cmake_minimum_required(VERSION 3.20)
project(gtest_template VERSION 1.0.0 LANGUAGES CXX)

add_compile_options(
    "$<$<AND:$<CONFIG:Release>,$<COMPILE_LANG_AND_ID:CXX,GNU,Clang,AppleClang,IntelLLVM>>:-O2>"
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(calculator STATIC src/calculator.cpp)
target_include_directories(calculator PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_compile_features(calculator PUBLIC cxx_std_17)
set_target_properties(calculator PROPERTIES CXX_EXTENSIONS OFF)

# BUILD_TESTING can be disabled when only the library is needed.
include(CTest)
if(BUILD_TESTING)
    include(FetchContent)
    # Match the default MSVC runtime used by the parent project.
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.15.2
        GIT_SHALLOW TRUE
    )
    FetchContent_MakeAvailable(googletest)

    add_executable(calculator_tests tests/calculator_test.cpp)
    target_link_libraries(calculator_tests PRIVATE calculator GTest::gtest_main)
    set_target_properties(calculator_tests PROPERTIES CXX_EXTENSIONS OFF)

    if(CMAKE_CROSSCOMPILING)
        message(STATUS "Cross-compiling calculator_tests: copy it to the target and run ./calculator_tests there; host CTest registration is disabled.")
    else()
        include(GoogleTest)
        gtest_discover_tests(calculator_tests)
    endif()
endif()
