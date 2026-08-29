add_executable(alkali_core_tests
    tst_core.cpp
)

target_link_libraries(alkali_core_tests PRIVATE
    Alkali::Core
    Qt6::Test
)

add_test(
    NAME alkali_core_tests
    COMMAND alkali_core_tests
)