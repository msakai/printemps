
file(GLOB TEST_SOURCE ${TOP_DIR}/test/extra/maxsat_evaluation/*.cpp)

add_executable(
    test_extra_maxsat_evaluation
    ${TEST_SOURCE}
)

target_include_directories(
    test_extra_maxsat_evaluation
    PUBLIC ${TOP_DIR}/printemps/
    PUBLIC ${TOP_DIR}/
    PUBLIC ${TOP_DIR}/external/include/
)

target_link_libraries(
    test_extra_maxsat_evaluation
    PUBLIC gtest
    PUBLIC gtest_main
    PUBLIC pthread
)

add_test(
    NAME test_extra_maxsat_evaluation
    COMMAND test_extra_maxsat_evaluation --gtest_output=xml
)
