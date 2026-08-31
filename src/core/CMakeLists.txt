add_library(alkali_core STATIC
    Frame.cpp
    Frame.h
    Image.cpp
    Image.h
    ImageReader.h
    ImageReader.cpp
    Rect.cpp
    Rect.h
)

add_library(Alkali::Core ALIAS alkali_core)

target_include_directories(alkali_core PUBLIC
    "${PROJECT_SOURCE_DIR}/src"
)

target_link_libraries(alkali_core PRIVATE
    alkali_sanitizers
    OpenImageIO::OpenImageIO
)
