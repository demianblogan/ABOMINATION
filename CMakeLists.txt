cmake_minimum_required(VERSION 3.31)

project(Abomination
    VERSION 0.1.0
    DESCRIPTION "A retro first-person shooter inspired by Quake (1996)"
    LANGUAGES CXX
)

if(NOT MSVC)
    message(FATAL_ERROR "Abomination supports only MSVC (Visual Studio) on Windows.")
endif()

# --- Language standard -------------------------------------------------------
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_SCAN_FOR_MODULES OFF) # We do not use C++20 modules; skip the dependency scan

# --- Project-wide helpers ----------------------------------------------------
include(CMake/CompilerOptions.cmake)

# --- Game executable ---------------------------------------------------------
add_executable(Abomination
    Source/Main.cpp
)
abomination_set_compiler_options(Abomination)

# Make the game the default project to run in Visual Studio (F5).
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Abomination)
