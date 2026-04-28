
file(GLOB TEST_SOURCE ${TOP_DIR}/test/wcnf/*.cpp)

add_executable(
    test_wcnf
    ${TEST_SOURCE}
)

target_include_directories(
    test_wcnf
    PUBLIC ${TOP_DIR}/printemps/
    PUBLIC ${TOP_DIR}/external/include/
)

target_link_libraries(
    test_wcnf
    PUBLIC gtest
    PUBLIC gtest_main
    PUBLIC pthread
)

add_test(
    NAME test_wcnf
    COMMAND test_wcnf --gtest_output=xml
)
