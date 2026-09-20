add_executable(alkali_rect_tests
    tst_rect.cpp
)

target_link_libraries(alkali_rect_tests PRIVATE
    Alkali::Core
    Qt6::Test
)

add_test(
    NAME alkali_rect_tests
    COMMAND alkali_rect_tests
)

add_executable(alkali_image_reader_tests
    tst_image_reader.cpp
)

target_link_libraries(alkali_image_reader_tests PRIVATE
    Alkali::Core
    Qt6::Test
)

add_test(
    NAME alkali_image_reader_tests
    COMMAND alkali_image_reader_tests
)
