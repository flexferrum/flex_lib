set(LAZY_LIST_HEADERS 
        ../../flex_lib/lazy_list.h
        ../../flex_lib/lazy_list.inl.h
)

set(LAZY_LIST_SOURCES)

set(LAZY_LIST_TEST_SOURCES
        lazy_list.cpp
    )

add_executable(lazy_list_tests
    ${LAZY_LIST_HEADERS}
    ${LAZY_LIST_SOURCES}
    ${LAZY_LIST_TEST_SOURCES}
    )

add_test(lazy_list_tests lazy_list_tests)
