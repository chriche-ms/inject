cmake_minimum_required(VERSION 3.13)

project(inject LANGUAGES CXX)

include(FetchContent)
include(cmake/FetchContent_MakeAvailable.cmake)

FetchContent_Declare(googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        release-1.10.0)

# Windows - ensure gtest is built with /MD or /MDd run-time library flags
set(gtest_force_shared_crt ON CACHE BOOL "GoogleTest: Use DLL-specific run-time" FORCE)

FetchContent_MakeAvailable(googletest)

add_subdirectory(inject)
add_subdirectory(inject_test)
