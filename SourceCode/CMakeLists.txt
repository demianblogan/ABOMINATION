# --- Generated sources -------------------------------------------------------
# The version lives only in project(... VERSION ...) of the root CMakeLists.txt.
# It is written into a generated .cpp (not a header), so bumping the version
# recompiles one file instead of everything that includes Version.h.
set(GENERATED_DIR ${CMAKE_CURRENT_BINARY_DIR}/Generated)
configure_file(Core/Version.cpp.in ${GENERATED_DIR}/Core/Version.cpp @ONLY)

# --- Game library: all game code except the entry point ----------------------
add_library(AbominationCore STATIC
    Core/Version.h
    Core/Version.cpp.in
    ${GENERATED_DIR}/Core/Version.cpp
)
target_include_directories(AbominationCore PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
abomination_set_compiler_options(AbominationCore)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES Core/Version.h Core/Version.cpp.in)
source_group(Generated FILES ${GENERATED_DIR}/Core/Version.cpp)

# --- Game executable: only the entry point -----------------------------------
add_executable(Abomination
    Main.cpp
)
target_link_libraries(Abomination PRIVATE AbominationCore)
abomination_set_compiler_options(Abomination)
