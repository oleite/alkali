cmake_minimum_required(VERSION 4.1)

project(Alkali VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


find_package(Qt6 6.8 REQUIRED COMPONENTS Quick QuickControls2)
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
        src/ImageRenderItem.h
        src/ImageRenderItem.cpp
        src/Processor.h
        src/Processor.cpp
        src/Paths.h
        src/Paths.cpp

    QML_FILES
        qml/Main.qml
        qml/areas/Area.qml
        qml/areas/FilledArea.qml
        qml/areas/ViewerArea.qml
        qml/areas/Viewport.qml
        qml/components/StatusBar.qml
        qml/components/AlkSplitView.qml
)

set_target_properties(alkali PROPERTIES
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(alkali PRIVATE 
    Qt6::Quick
    Qt6::QuickControls2
    Halide::Halide
)

option(ALKALI_ENABLE_ASAN "Enable AddressSanitizer" OFF)

if(ALKALI_ENABLE_ASAN)
    if(MSVC)
        target_compile_options(alkali PRIVATE /fsanitize=address /Zi)
        target_link_options(alkali PRIVATE /INCREMENTAL:NO)
    else()
        target_compile_options(
            alkali PRIVATE
            -fsanitize=address
            -fno-omit-frame-pointer
        )
        target_link_options(
            alkali PRIVATE
            -fsanitize=address
        )
    endif()
endif()

if(APPLE)
    file(GLOB RESOURCE_ENTRIES
        CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/resources/*"
    )

    target_sources(alkali PRIVATE ${RESOURCE_ENTRIES})

    set_source_files_properties(
        ${RESOURCE_ENTRIES}
        PROPERTIES
            MACOSX_PACKAGE_LOCATION "Resources"
    )
else()
    add_custom_command(
        TARGET alkali POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different
            "${CMAKE_CURRENT_SOURCE_DIR}/resources"
            "$<TARGET_FILE_DIR:alkali>/resources"
    )
endif()

include(GNUInstallDirs)
install(TARGETS alkali
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
