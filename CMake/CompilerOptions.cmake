# Compiler options shared by all Abomination targets.
# Applied per target (PRIVATE), so third-party code is never affected.

function(abomination_set_compiler_options target)
    target_compile_options(${target} PRIVATE
        /W4                     # High warning level
        /WX                     # Treat warnings as errors
        /permissive-            # Strict standard conformance
        /utf-8                  # Source and execution character sets are UTF-8
        /Zc:__cplusplus         # Report the real standard in __cplusplus
        /external:anglebrackets # Headers included with <...> are external...
        /external:W0            # ...and produce no warnings
        /MP                     # Compile source files in parallel
    )
endfunction()
