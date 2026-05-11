#██████   █████    ███████      █████████    █████████  █████ ███████████    ███████████   ██████████ ██████████
#░░██████ ░░███   ███░░░░░███   ███░░░░░███  ███░░░░░███░░███ ░█░░░███░░░█   ░░███░░░░░███ ░░███░░░░░█░░███░░░░███
# ░███░███ ░███  ███     ░░███ ███     ░░░  ███     ░░░  ░███ ░   ░███  ░     ░███    ░███  ░███  █ ░  ░███   ░░███
# ░███░░███░███ ░███      ░███░███         ░███          ░███     ░███        ░██████████   ░██████    ░███    ░███
# ░███ ░░██████ ░███      ░███░███    █████░███    █████ ░███     ░███        ░███░░░░░███  ░███░░█    ░███    ░███
# ░███  ░░█████ ░░███     ███ ░░███  ░░███ ░░███  ░░███  ░███     ░███        ░███    ░███  ░███ ░   █ ░███    ███
# █████  ░░█████ ░░░███████░   ░░█████████  ░░█████████  █████    █████       █████   █████ ██████████ ██████████
#░░░░░    ░░░░░    ░░░░░░░      ░░░░░░░░░    ░░░░░░░░░  ░░░░░    ░░░░░       ░░░░░   ░░░░░ ░░░░░░░░░░ ░░░░░░░░░░

cmake_minimum_required(VERSION 4.0)

project(Noggit
VERSION 1.0
LANGUAGES C CXX
)


# Global Configuration
cmake_policy(SET CMP0057 NEW)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")

set(EXTERNAL_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/external")

set_property(GLOBAL PROPERTY USE_FOLDERS ON)


# Includes
include(CheckCXXCompilerFlag)
include(cmake_function)
include(cmake_macro)
include(FetchContent)
include(FetchContentFast)

# Options
option(FAST_BUILD_FETCHCONTENT "Skip FetchContent dependency checks" OFF)
option(USE_SQL "Enable SQL UID save" OFF)
option(VALIDATE_OPENGL_PROGRAMS "Validate OpenGL programs" OFF)
option(NOGGIT_BUILD_NODE_DATAMODELS "Build node data models" ON)
option(NOGGIT_ENABLE_TRACY_PROFILER "Enable Tracy profiler" OFF)
option(NOGGIT_ALL_WARNINGS "Enable all warnings" OFF)
option(NOGGIT_LOGTOCONSOLE "Log to console instead of file" OFF)
option(NOGGIT_OPENGL_ERROR_CHECK "Enable OpenGL error checks" ON)

option(FAST_BUILD_LIB_PCH "Use PCH for libraries" OFF)
option(FAST_BUILD_LIB_JUMBO "Use unity builds for libraries" OFF)
option(FAST_BUILD_NOGGIT_PCH "Use PCH for Noggit" OFF)
option(FAST_BUILD_NOGGIT_JUMBO "Use unity builds for Noggit" OFF)

# Platform Setup
includePlatform("prefind")

if(UNIX AND NOT APPLE)
set(OpenGL_GL_PREFERENCE GLVND)
endif()

# Dependencies
find_package(Lua REQUIRED)
find_package(StormLib REQUIRED)
find_package(CascLib REQUIRED)
find_package(OpenGL REQUIRED)
find_package(Json REQUIRED)
find_package(lodepng REQUIRED)
find_package(FastNoise2 REQUIRED)
find_package(Sol2 REQUIRED)
find_package(Qt5 REQUIRED COMPONENTS
Widgets
OpenGL
OpenGLExtensions
Network
Xml
Multimedia
)

set(FASTNOISE2_NOISETOOL OFF CACHE BOOL "")
option(BUILD_LIBNOISE_EXAMPLES "Build libnoise examples" OFF)

# External Libraries
add_subdirectory("${EXTERNAL_SOURCE_DIR}/qt-color-widgets")
add_subdirectory("${EXTERNAL_SOURCE_DIR}/qtimgui")
add_subdirectory("${EXTERNAL_SOURCE_DIR}/QtAdvancedDockingSystem")
add_subdirectory("${EXTERNAL_SOURCE_DIR}/NodeEditor")
add_subdirectory("${EXTERNAL_SOURCE_DIR}/libnoise")
if(TARGET noise-static)
target_include_directories(noise-static
    PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/external/libnoise/src
)
endif()

add_subdirectory("${EXTERNAL_SOURCE_DIR}/glm")

if(WIN32)
add_subdirectory("${EXTERNAL_SOURCE_DIR}/framelesshelper")
endif()


# Source Collection
collect_files(math_sources src/math FALSE "*.cpp" "")
collect_files(opengl_sources src/opengl FALSE "*.cpp" "")
collect_files(util_sources src/util TRUE "*.c;*.cpp" "")
collect_files(util_headers src/util TRUE "*.h;*.hpp" "")

if(NOT NOGGIT_BUILD_NODE_DATAMODELS)
collect_files(noggit_root_sources src/noggit TRUE "*.cpp"
"NodeEditor/Nodes/Containers;NodeEditor/Nodes/Data;NodeEditor/Nodes/Functions;NodeEditor/Nodes/Math;NodeEditor/Nodes/World")
else()
collect_files(noggit_root_sources src/noggit TRUE "*.cpp" "")
endif()

collect_files(noggit_root_headers src/noggit TRUE "*.h;*.hpp;*.inl" "")
collect_files(math_headers src/math FALSE "*.h;*.hpp" "")
collect_files(opengl_headers src/opengl FALSE "*.h;*.hpp" "")
collect_files(shaders src/glsl FALSE "*.glsl" "")

collect_files(png_blp_sources src/external/PNG2BLP TRUE "*.c;*.cpp" "")
collect_files(png_blp_headers src/external/PNG2BLP TRUE "*.h;*.hpp" "")

collect_files(imguizmo_sources src/external/imguizmo FALSE "*.c;*.cpp" "")
collect_files(imguizmo_headers src/external/imguizmo FALSE "*.h;*.hpp" "")

collect_files(imguipiemenu_sources src/external/imguipiemenu FALSE "*.c;*.cpp" "")
collect_files(imguipiemenu_headers src/external/imguipiemenu FALSE "*.h;*.hpp" "")

collect_files(gradienteditor_sources src/external/qtgradienteditor FALSE "*.c;*.cpp" "")
collect_files(gradienteditor_headers src/external/qtgradienteditor FALSE "*.h;*.hpp" "")

collect_files(tracy_sources src/external/tracy FALSE "*.c;*.cpp" "")
collect_files(tracy_headers src/external/tracy FALSE "*.h;*.hpp" "")

collect_files(archive_sources src/external/blizzard-archive-library/src FALSE "*.c;*.cpp" "")
collect_files(archive_headers src/external/blizzard-archive-library/include FALSE "*.h;*.hpp" "")

collect_files(database_sources src/external/blizzard-database-library/src TRUE "*.c;*.cpp" "")
collect_files(database_headers src/external/blizzard-database-library/include TRUE "*.h;*.hpp" "")

if(WIN32)
collect_files(os_sources include/win FALSE "*.c;*.cpp" "")
collect_files(os_headers include/win FALSE "*.h;*.hpp" "")
endif()

collect_files(resource_files resources FALSE "*.qrc" "")

# Libraries
add_library(blizzard-archive-library STATIC)
target_sources(blizzard-archive-library PRIVATE
    ${CMAKE_SOURCE_DIR}/src/external/blizzard-archive-library/src/CASCArchive.cpp
    ${CMAKE_SOURCE_DIR}/src/external/blizzard-archive-library/src/BaseArchive.cpp
    ${CMAKE_SOURCE_DIR}/src/external/blizzard-archive-library/src/ClientData.cpp
)
target_include_directories(blizzard-archive-library PUBLIC
    ${CMAKE_SOURCE_DIR}/src/external/blizzard-archive-library/include
    ${CMAKE_SOURCE_DIR}/src/external/blizzard-archive-library/src
)
target_compile_features(blizzard-archive-library PUBLIC cxx_std_20)

add_library(blizzard-database-library STATIC)
file(GLOB_RECURSE DB_SOURCES
    CONFIGURE_DEPENDS
    ${CMAKE_SOURCE_DIR}/src/external/blizzard-database-library/src/*.cpp
)
target_sources(blizzard-database-library PRIVATE ${DB_SOURCES})
target_include_directories(blizzard-database-library PUBLIC
    ${CMAKE_SOURCE_DIR}/src/external/blizzard-database-library/include
)
target_compile_features(blizzard-database-library PUBLIC cxx_std_20)

# Main Executable
if(WIN32)
set(_noggit_platform_flags WIN32)
elseif(APPLE)
set(_noggit_platform_flags MACOSX_BUNDLE)
else()
set(_noggit_platform_flags "")
endif()

add_executable(noggit
${_noggit_platform_flags}
)

# Sources
target_sources(noggit PRIVATE
${noggit_root_sources}
${noggit_root_headers}
${math_sources}
${math_headers}
${opengl_sources}
${opengl_headers}
${util_sources}
${util_headers}
${png_blp_sources}
${png_blp_headers}
${imguizmo_sources}
${imguizmo_headers}
${imguipiemenu_sources}
${imguipiemenu_headers}
${gradienteditor_sources}
${gradienteditor_headers}
${tracy_sources}
${tracy_headers}
${resource_files}
${shaders}
${os_sources}
${os_headers}
)

set(_noggit_revision_output_dir "${CMAKE_BINARY_DIR}/revision_output")
set(_noggit_revision_output_file "${_noggit_revision_output_dir}/revision.h")

set_source_files_properties(
    "${_noggit_revision_output_file}"
    PROPERTIES GENERATED TRUE
)

# Includes
target_include_directories(noggit
PRIVATE
src
include
${CMAKE_CURRENT_BINARY_DIR}
${Qt5Gui_PRIVATE_INCLUDE_DIRS}
${_noggit_revision_output_dir}


SYSTEM PRIVATE
    src/external/PNG2BLP
    src/external/PNG2BLP/libimagequant
    src/external/PNG2BLP/libpng
    src/external/PNG2BLP/libtxc_dxtn
    src/external/PNG2BLP/pngpp
    src/external/PNG2BLP/zlib
    src/external/imguizmo
    src/external/imguipiemenu
    src/external/libnoise/src
    src/external/qtgradienteditor
    src/external/tracy
    src/external/blizzard-database-library/include
    src/external/blizzard-archive-library/include


)

# Compiler Definitions
if(VALIDATE_OPENGL_PROGRAMS)
target_compile_definitions(noggit PRIVATE
VALIDATE_OPENGL_PROGRAMS
)
endif()

if(NOGGIT_ENABLE_TRACY_PROFILER)
target_compile_definitions(noggit PRIVATE
TRACY_ENABLE
)
endif()

if(NOT NOGGIT_BUILD_NODE_DATAMODELS)
target_compile_definitions(noggit PRIVATE
DO_NOT_BUILD_NODES
)
endif()

if(NOGGIT_LOGTOCONSOLE)
target_compile_definitions(noggit PRIVATE
DEBUG__LOGGINGTOCONSOLE
)
endif()

if(NOT NOGGIT_OPENGL_ERROR_CHECK)
target_compile_definitions(noggit PRIVATE
NOGGIT_DO_NOT_CHECK_FOR_OPENGL_ERRORS
)
endif()

# Compiler Options
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
target_compile_options(noggit PRIVATE
$<$<COMPILE_LANGUAGE:C>:-Wno-old-style-definition>
-Wno-variadic-macros
-Wno-vla
-Wno-date-time
-Wno-padded
-Wno-multichar
-Werror=mismatched-tags
)
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
target_compile_options(noggit PRIVATE
-Wno-c++98-compat
-Wno-c++98-compat-pedantic
-Wno-gnu-anonymous-struct
-Wno-vla-extension
-Wno-zero-length-array
-Wno-gnu-zero-variadic-macro-arguments
-Wno-nested-anon-types
-Wno-four-char-constants
-Wno-exit-time-destructors
-Wno-global-constructors
-Wno-disabled-macro-expansion
-Wno-weak-vtables
-Wno-weak-template-vtables
)
endif()

if(MSVC)
target_compile_options(noggit PRIVATE
/EHa
/MP
/bigobj
/we4099
)
endif()

# Link Libraries
target_link_libraries(blizzard-archive-library PRIVATE
StormLib
CascLib
)

target_link_libraries(noggit PRIVATE
${OPENGL_LIBRARIES}
StormLib
CascLib


Qt5::Widgets
Qt5::OpenGL
Qt5::OpenGLExtensions
Qt5::Xml
Qt5::Network
Qt5::Multimedia

ColorWidgets-qt5
qt_imgui_widgets
qtadvanceddocking
nodes
noise-static
noiseutils-static
glm
lodepng
FastNoise
sol2::sane
nlohmann_json::nlohmann_json

blizzard-archive-library
blizzard-database-library


)

if(WIN32)
target_link_libraries(noggit PRIVATE
FramelessHelper
)
endif()

# Platform Libraries
if(APPLE)
target_link_libraries(noggit PRIVATE
"-framework Cocoa"
"-framework AppKit"
"-framework Foundation"
)
endif()

if(UNIX)
find_package(BZip2 REQUIRED)


target_link_libraries(noggit PRIVATE
    ${BZIP2_LIBRARIES}
)


endif()

# Qt
set_property(TARGET noggit PROPERTY AUTOMOC ON)

# Distribution Assets
add_custom_command(TARGET noggit POST_BUILD
COMMAND ${CMAKE_COMMAND} -E copy_directory
"${PROJECT_SOURCE_DIR}/dist/themes"
"$<TARGET_FILE_DIR:noggit>/themes"
)

add_custom_command(TARGET noggit POST_BUILD
COMMAND ${CMAKE_COMMAND} -E copy_directory
"${PROJECT_SOURCE_DIR}/dist/listfile"
"$<TARGET_FILE_DIR:noggit>"
)

add_custom_command(TARGET noggit POST_BUILD
COMMAND ${CMAKE_COMMAND} -E copy_directory
"${PROJECT_SOURCE_DIR}/dist/definitions"
"$<TARGET_FILE_DIR:noggit>/definitions"
)

# Git Revision
set(_noggit_revision_output_dir "${CMAKE_BINARY_DIR}/revision_output")
set(_noggit_revision_template_file "${CMAKE_SOURCE_DIR}/cmake/revision.h.in")
set(_noggit_revision_output_file "${_noggit_revision_output_dir}/revision.h")
set(_noggit_revision_state_file "${CMAKE_BINARY_DIR}/revision.state")
set(_noggit_revision_script_file "${CMAKE_SOURCE_DIR}/cmake/GenerateRevision.cmake")

find_package(Git)

if(GIT_FOUND)
add_custom_target(update_git_revision
ALL
DEPENDS
"${_noggit_revision_template_file}"
"${_noggit_revision_script_file}"


    BYPRODUCTS
        "${_noggit_revision_output_file}"
        "${_noggit_revision_state_file}"

    COMMAND ${CMAKE_COMMAND}
        -D_noggit_revision_template_file="${_noggit_revision_template_file}"
        -D_noggit_revision_output_file="${_noggit_revision_output_file}"
        -D_noggit_revision_state_file="${_noggit_revision_state_file}"
        -DGIT_EXECUTABLE="${GIT_EXECUTABLE}"
        -P "${_noggit_revision_script_file}"

    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
)

add_dependencies(noggit update_git_revision)


endif()

# QT Build Options
file(GLOB_RECURSE UI_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/noggit/ui/*.ui")
set(UI_DIRS "")
foreach(UI_FILE ${UI_FILES})
    get_filename_component(UI_DIR ${UI_FILE} DIRECTORY)
    list(APPEND UI_DIRS ${UI_DIR})
endforeach()
list(REMOVE_DUPLICATES UI_DIRS)

set_target_properties(noggit PROPERTIES
    AUTOMOC ON
    AUTOUIC ON
    AUTORCC ON
    AUTOUIC_SEARCH_PATHS "${UI_DIRS}"
)

# Fast Build Options
if(FAST_BUILD_LIB_JUMBO)
set_target_properties(noise-static PROPERTIES UNITY_BUILD ON)
set_target_properties(noiseutils-static PROPERTIES UNITY_BUILD ON)
set_target_properties(qt_imgui_widgets PROPERTIES UNITY_BUILD ON)
set_target_properties(qtadvanceddocking PROPERTIES UNITY_BUILD ON)
set_target_properties(ColorWidgets-qt5 PROPERTIES UNITY_BUILD ON)
set_target_properties(blizzard-archive-library PROPERTIES UNITY_BUILD ON)
set_target_properties(blizzard-database-library PROPERTIES UNITY_BUILD ON)
endif()

if(FAST_BUILD_NOGGIT_JUMBO)
set_target_properties(noggit PROPERTIES UNITY_BUILD ON)
endif()

# Platform Post Setup
includePlatform("postfind")
includePlatform("pack")
