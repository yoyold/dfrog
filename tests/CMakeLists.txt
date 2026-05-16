find_package(GTest CONFIG REQUIRED)
include(GoogleTest)

add_executable(dfrog_unit_tests
    smoke_test.cpp
    version_test.cpp
)
target_link_libraries(dfrog_unit_tests
    PRIVATE
        dfrog_core
        GTest::gtest
        GTest::gtest_main
)
dfrog_set_warnings(dfrog_unit_tests)
dfrog_apply_sanitizers(dfrog_unit_tests)

gtest_discover_tests(dfrog_unit_tests
    DISCOVERY_MODE PRE_TEST
    PROPERTIES
        LABELS "unit"
)
