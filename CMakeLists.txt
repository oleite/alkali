cmake_minimum_required(VERSION 4.1)

project(
    Alkali
    VERSION 0.1
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(CTest)
include(GNUInstallDirs)

option(ALKALI_ENABLE_ASAN "Enable AddressSanitizer" OFF)


# Dependencies

find_package(Qt6 6.8 REQUIRED COMPONENTS
    Core
    Gui
    Quick
    QuickControls2
)

if(BUILD_TESTING)
    find_package(Qt6 6.8 REQUIRED COMPONENTS Test)
endif()

find_package(Halide REQUIRED)
find_package(OpenColorIO REQUIRED)
find_package(OpenImageIO CONFIG REQUIRED)

qt_standard_project_setup(REQUIRES 6.8)


# Project configuration

include(cmake/Sanitizers.cmake)


# Targets

add_subdirectory(src)

if(BUILD_TESTING)
    add_subdirectory(tests)
endif()