# vcpkg triplet of the project: how every library from vcpkg.json is built.
# Same as the built-in x64-windows-static-md, plus project-specific options for individual ports.

# 64-bit Windows.
set(VCPKG_TARGET_ARCHITECTURE x64)

# Libraries are linked statically into the executable, the C++ runtime (CRT) is linked dynamically (/MD).
set(VCPKG_CRT_LINKAGE dynamic)
set(VCPKG_LIBRARY_LINKAGE static)

# glad generates headers for the OpenGL Core profile instead of Compatibility:
# functions removed from modern OpenGL (glBegin, glMatrixMode, ...) are not even declared.
if(PORT STREQUAL "glad")
    set(GLAD_PROFILE "core")
endif()
