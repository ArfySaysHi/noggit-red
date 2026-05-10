# This file is part of Noggit3, licensed under GNU General Public License (version 3).
# Dependency: json.hpp
include(FetchContent)

FetchContent_Declare(
  json
  GIT_REPOSITORY https://github.com/nlohmann/json
  GIT_TAG v3.11.3
)

FetchContent_MakeAvailable(json)
