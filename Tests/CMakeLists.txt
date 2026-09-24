find_package(GTest CONFIG REQUIRED)

# --- Unit tests: one executable for all tests, mirrors SourceCode/ ------------
set(TEST_SOURCES
    Core/FrameTimerTests.cpp
    Core/LogTests.cpp
    Core/VersionTests.cpp
)

add_executable(AbominationTests
    ${TEST_SOURCES}
)
target_link_libraries(AbominationTests PRIVATE
    AbominationCore
    GTest::gtest
    GTest::gtest_main   # Provides main() that runs all tests
)
abomination_set_compiler_options(AbominationTests)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${TEST_SOURCES})

# --- CTest integration -------------------------------------------------------
# Registers every TEST(...) as a separate CTest test. PRE_TEST lists the tests
# when CTest runs instead of after every build, so building never runs test code.
include(GoogleTest)
gtest_discover_tests(AbominationTests DISCOVERY_MODE PRE_TEST)
