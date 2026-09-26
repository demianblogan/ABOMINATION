find_package(GTest CONFIG REQUIRED)
find_package(Stb REQUIRED)  # ImageTests writes test PNG files with stb_image_write

# --- Unit tests: one executable for all tests, mirrors SourceCode/ ------------
set(TEST_SOURCES
    Core/AssetCacheTests.cpp
    Core/FileSystemTests.cpp
    Core/FixedTimestepTests.cpp
    Core/FrameLimiterTests.cpp
    Core/FrameStatisticsTests.cpp
    Core/FrameTimerTests.cpp
    Core/ImageTests.cpp
    Core/LogTests.cpp
    Core/TransformTests.cpp
    Core/VersionTests.cpp

    Gameplay/FreeFlyCameraControllerTests.cpp
    Gameplay/SpinTests.cpp

    Input/ActionStatesTests.cpp
    Input/InputBindingsTests.cpp
    Input/KeyboardTests.cpp
    Input/MouseTests.cpp

    Renderer/CameraTests.cpp
    Renderer/GLTextureTests.cpp
    Renderer/MeshPrimitivesTests.cpp

    ThirdParty/EnTTTests.cpp
)

add_executable(AbominationTests
    ${TEST_SOURCES}
)
target_link_libraries(AbominationTests PRIVATE
    AbominationCore
    GTest::gtest
    GTest::gtest_main   # Provides main() that runs all tests
)
target_include_directories(AbominationTests SYSTEM PRIVATE ${Stb_INCLUDE_DIR})
abomination_set_compiler_options(AbominationTests)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${TEST_SOURCES})

# --- CTest integration -------------------------------------------------------
# Registers every TEST(...) as a separate CTest test. PRE_TEST lists the tests
# when CTest runs instead of after every build, so building never runs test code.
include(GoogleTest)
gtest_discover_tests(AbominationTests DISCOVERY_MODE PRE_TEST)
