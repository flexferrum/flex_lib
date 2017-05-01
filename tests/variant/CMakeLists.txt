project (variant_tests)

include (collect_sources)

CollectSources(Sources Headers ${CMAKE_CURRENT_SOURCE_DIR}  ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(${PROJECT_NAME}
    ${Sources}
    ${Headers}
    )

target_link_libraries(${PROJECT_NAME} ${GTEST_BOTH_LIBRARIES} flex_lib)

add_test(${PROJECT_NAME} ${PROJECT_NAME})
