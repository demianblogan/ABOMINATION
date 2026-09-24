# --- Dependencies ------------------------------------------------------------
find_package(SDL3 CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)

# --- Generated sources -------------------------------------------------------
# The version lives only in project(... VERSION ...) of the root CMakeLists.txt.
# It is written into a generated .cpp (not a header), so bumping the version
# recompiles one file instead of everything that includes Version.h.
set(GENERATED_DIR ${CMAKE_CURRENT_BINARY_DIR}/Generated)
configure_file(Core/Version.cpp.in ${GENERATED_DIR}/Core/Version.cpp @ONLY)

# --- Game library: all game code except the entry point ----------------------
set(LIBRARY_SOURCES
    Application/Application.h
    Application/Application.cpp

    Core/BuildConfiguration.h
    Core/Log.h
    Core/Log.cpp
    Core/Version.h
    Core/Version.cpp.in

    Platform/SDLLibrary.h
    Platform/SDLLibrary.cpp
    Platform/SystemServices.h
    Platform/SystemServices.cpp
    Platform/Window.h
    Platform/Window.cpp
)

add_library(AbominationCore STATIC
    ${LIBRARY_SOURCES}
    ${GENERATED_DIR}/Core/Version.cpp
)
target_include_directories(AbominationCore PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(AbominationCore PRIVATE
    SDL3::SDL3      # PRIVATE: only Platform/*.cpp include SDL, no header does
    spdlog::spdlog  # PRIVATE: only Log.cpp includes spdlog, Log.h does not
)
abomination_set_compiler_options(AbominationCore)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${LIBRARY_SOURCES})
source_group(Generated FILES ${GENERATED_DIR}/Core/Version.cpp)

# --- Game executable: only the entry point -----------------------------------
add_executable(Abomination
    Main.cpp
)
target_link_libraries(Abomination PRIVATE AbominationCore)
abomination_set_compiler_options(Abomination)
