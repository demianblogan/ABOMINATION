# GLAD 2: the OpenGL 4.6 Core function loader.
# The files are generated once by https://gen.glad.sh and are not edited by hand;
# the settings they were generated with are listed at the top of include/glad/gl.h.
# To change them (another OpenGL version, extensions), generate the files again and replace them.

add_library(GLAD STATIC
    include/glad/gl.h
    include/KHR/khrplatform.h
    src/gl.c
)

# SYSTEM: the headers are treated as external, so they produce no warnings in our code (/external:W0).
target_include_directories(GLAD SYSTEM PUBLIC include)

# Group the target under "ThirdParty" in the Visual Studio solution.
set_target_properties(GLAD PROPERTIES FOLDER ThirdParty)
