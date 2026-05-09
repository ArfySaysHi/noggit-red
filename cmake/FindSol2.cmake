# This file is part of Noggit3, licensed under GNU General Public License (version 3).

# Dependency: sol2
FetchContent_Declare (sol2
        GIT_REPOSITORY https://github.com/ThePhD/sol2
        GIT_TAG v3.3.0
        )
FetchContent_MakeAvailable (sol2)
# sol2::sol2 neither links lua nor sets include directories as system so will clobber us with
# loads of warnings, sadly. It also wants to be install(EXPORT)ed which is not what we want.
add_library (sane-sol2 INTERFACE)
add_library (sol2::sane ALIAS sane-sol2)
target_link_libraries (sane-sol2 INTERFACE Lua::Lua)
target_include_directories (sane-sol2 SYSTEM INTERFACE "${sol2_SOURCE_DIR}/include")
#
# Patch sol2's optional_implementation.hpp for GCC C++20 compatibility.
# The emplace() specialisation for optional<T&> calls this->construct()
# which doesn't exist on the reference specialisation.
file(READ "${sol2_SOURCE_DIR}/include/sol/optional_implementation.hpp" _sol2_optional_content)
string(REPLACE
    "this->construct(std::forward<Args>(args)...);"
    "*this = sol::optional<T&>(std::forward<Args>(args)...);"
    _sol2_optional_content "${_sol2_optional_content}")
file(WRITE "${sol2_SOURCE_DIR}/include/sol/optional_implementation.hpp" "${_sol2_optional_content}")
