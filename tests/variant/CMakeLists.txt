set(VARIANT_HEADERS 
        ../../flex_lib/variant.h
        ../../flex_lib/variant/variant_impl.h
#        ../../flex_lib/lazy_list.inl.h
)

set(VARIANT_SOURCES)

set(VARIANT_TEST_SOURCES
        variant.cpp
    )

add_executable(variant_tests
    ${VARIANT_HEADERS}
    ${VARIANT_SOURCES}
    ${VARIANT_TEST_SOURCES}
    )

add_test(variant_tests variant_tests)
