add_library(alkali_sanitizers INTERFACE)

if(ALKALI_ENABLE_ASAN)
    if(MSVC)
        target_compile_options(alkali_sanitizers INTERFACE
            /fsanitize=address
            /Zi
        )

        target_link_options(alkali_sanitizers INTERFACE
            /INCREMENTAL:NO
        )
    else()
        target_compile_options(alkali_sanitizers INTERFACE
            -fsanitize=address
            -fno-omit-frame-pointer
        )

        target_link_options(alkali_sanitizers INTERFACE
            -fsanitize=address
        )
    endif()
endif()