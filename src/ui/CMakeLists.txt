qt_add_executable(alkali
    ../main.cpp
)

target_include_directories(alkali PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/cpp"
)

target_compile_definitions(alkali PRIVATE
    QT_NO_KEYWORDS
)

target_link_libraries(alkali PRIVATE
    Alkali::Core
    alkali_sanitizers

    Qt6::Quick
    Qt6::QuickControls2

    Halide::Halide
    OpenColorIO::OpenColorIO
    OpenImageIO::OpenImageIO
)

qt_add_qml_module(alkali
    URI Alkali
    VERSION 1.0

    SOURCES
        cpp/ImageRenderItem.cpp
        cpp/ImageRenderItem.h
        cpp/Paths.cpp
        cpp/Paths.h
        cpp/Processor.cpp
        cpp/Processor.h

    QML_FILES
        Main.qml

        areas/Area.qml
        areas/FilledArea.qml
        areas/ViewerArea.qml
        areas/Viewport.qml

        components/AlkSplitView.qml
        components/StatusBar.qml
)


# Application bundle

set_target_properties(alkali PROPERTIES
    MACOSX_BUNDLE TRUE
    MACOSX_BUNDLE_BUNDLE_VERSION "${PROJECT_VERSION}"
    MACOSX_BUNDLE_SHORT_VERSION_STRING
        "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}"

    WIN32_EXECUTABLE TRUE
)


# Runtime resources

if(APPLE)
    file(
        GLOB RESOURCE_ENTRIES
        CONFIGURE_DEPENDS
        "${PROJECT_SOURCE_DIR}/resources/*"
    )

    target_sources(alkali PRIVATE
        ${RESOURCE_ENTRIES}
    )

    set_source_files_properties(
        ${RESOURCE_ENTRIES}
        PROPERTIES
            MACOSX_PACKAGE_LOCATION "Resources"
    )
else()
    add_custom_command(
        TARGET alkali
        POST_BUILD

        COMMAND
            ${CMAKE_COMMAND} -E copy_directory_if_different
            "${PROJECT_SOURCE_DIR}/resources"
            "$<TARGET_FILE_DIR:alkali>/resources"

        VERBATIM
    )
endif()


# Installation

install(
    TARGETS alkali

    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)