# This file is part of Noggit3, licensed under GNU General Public License (version 3).

# Dependency: FastNoise2
FetchContent_Declare (fastnoise2
        GIT_REPOSITORY https://github.com/tswow/FastNoise2.git
        GIT_TAG v0.0.1-heightmap
        PATCH_COMMAND "${CMAKE_COMMAND}" -P "${CMAKE_SOURCE_DIR}/cmake/deps/patch_fastnoise2.cmake"
        UPDATE_DISCONNECTED true
        )
include(FetchContent)

MESSAGE(STATUS "Installing FastNoise2...")

FetchContent_MakeAvailable(fastnoise2)

SET(FASTNOISE2_NOISETOOL:BOOL OFF)
SET(FASTNOISE2_TESTS:BOOL OFF)
