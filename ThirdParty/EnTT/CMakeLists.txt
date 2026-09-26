# EnTT 4.0.0: the entity-component-system library, header-only.
# include/entt/entt.hpp is the official single-header build from the release tag
# (https://github.com/skypjack/entt/blob/v4.0.0/single_include/entt/entt.hpp), not edited by hand.
# vcpkg only has 3.16 so far; once it has 4.x, this folder can be replaced by find_package(EnTT).
# To update: replace entt.hpp and LICENSE with the files of the new release tag.

# INTERFACE: there is nothing to compile, the target only passes its include folder to whoever links it.
add_library(EnTT INTERFACE)

# SYSTEM: the header is treated as external, so it produces no warnings in our code (/external:W0).
target_include_directories(EnTT SYSTEM INTERFACE include)
