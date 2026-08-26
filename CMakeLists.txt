cmake_minimum_required(VERSION 3.16)

project(Alkali VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 6.8 REQUIRED COMPONENTS Quick)
find_package(Halide REQUIRED)


qt_standard_project_setup(REQUIRES 6.8)

qt_add_executable(alkali
    src/main.cpp
)

target_include_directories(alkali PRIVATE
    src
)

target_compile_definitions(alkali PRIVATE QT_NO_KEYWORDS)

qt_add_qml_module(alkali
    URI Alkali
    VERSION 1.0

    SOURCES
        src/ImageViewer.h
        src/ImageViewer.cpp
        src/Processor.h
        src/Processor.cpp

    QML_FILES
        qml/Main.qml
        qml/areas/Area.qml
        qml/areas/FilledArea.qml
        qml/components/StatusBar.qml
        qml/components/AlkSplitView.qml

    RESOURCES
        qml/assets/sipi_mandril.png
        qml/assets/rgbtest.png
)

set_target_properties(alkali PROPERTIES
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(alkali PRIVATE 
    Qt6::Quick
    Halide::Halide
)

include(GNUInstallDirs)
install(TARGETS alkali
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
