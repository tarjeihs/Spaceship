cmake_minimum_required(VERSION 3.16)
project(Spaceship)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Use only Debug, Release, and Dist configurations
set(CMAKE_CONFIGURATION_TYPES "Debug;Release;Dist" CACHE STRING "" FORCE)

# This makes sure that CMake generates the appropriate files for VSCode
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
include(CMakeToolsHelpers OPTIONAL)

# Enable parallel compilation
add_compile_options(/MP)

# Output directories
set(OUTPUT_DIR ${CMAKE_BINARY_DIR}//Binaries)
set(INTERMEDIATE_DIR ${CMAKE_BINARY_DIR}//Intermediate)
set(ROCKET_DIR "D:/Workspace/Rocket")

# Collect all source files for the Game project
file(GLOB_RECURSE GAME_FILES
    Game/Source/*.cpp
    Game/Source/*.h
)

# Collect all HLSL and GLSL files for the Game project
file(GLOB_RECURSE GAME_SHADER_FILES 
    Game/Shaders/*.hlsl
    Game/Shaders/*.glsl
)

# Define source groups for Game
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/Game/Source PREFIX "Source" FILES ${GAME_FILES})

# Define source groups for shader files
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/Game/Shaders PREFIX "Shaders" FILES ${GAME_SHADER_FILES})

add_executable(Game ${GAME_FILES})

# Add Rocket as a subdirectory with a proper binary directory
add_subdirectory(${ROCKET_DIR} "Rocket" EXCLUDE_FROM_ALL)

target_include_directories(Game PRIVATE
    Game/Source
    Game/Vendor
    ${ROCKET_DIR}/Engine/Source
    ${ROCKET_DIR}/Engine/Vendor
)

target_link_libraries(Game PRIVATE
    Engine
)

set_source_files_properties(${GAME_SHADER_FILES} PROPERTIES HEADER_FILE_ONLY TRUE)
target_sources(Game PRIVATE ${GAME_SHADER_FILES})

target_compile_options(Game PRIVATE /std:c++20)

set_target_properties(Game PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${OUTPUT_DIR}/$<CONFIG>/Game
    LIBRARY_OUTPUT_DIRECTORY ${OUTPUT_DIR}/$<CONFIG>/Game
    RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_DIR}/$<CONFIG>/Game
    PCH_FILE GamePCH.h
    PCH_SOURCE Game/Source/GamePCH.cpp
)

if(WIN32)
    target_compile_definitions(Game PRIVATE
        RK_PLATFORM_WINDOWS
        RK_BUILD_DLL
    )
endif()

# Configuration-specific flags
set(CMAKE_CXX_FLAGS_DEBUG "/D_DEBUG /MDd /Zi /Ob0 /Od /RTC1")
set(CMAKE_CXX_FLAGS_RELEASE "/MD /O2 /Ob2 /DNDEBUG")
set(CMAKE_CXX_FLAGS_DIST "/MD /O2 /Ob2 /DNDEBUG")

# Configuration-specific linker flags
set(CMAKE_EXE_LINKER_FLAGS_DEBUG "/DEBUG")
set(CMAKE_EXE_LINKER_FLAGS_RELEASE "")
set(CMAKE_EXE_LINKER_FLAGS_DIST "")

# Configuration-specific defines
# Configuration-specific defines
foreach(CONFIG_TYPE IN ITEMS Debug Release Dist)
    string(TOUPPER ${CONFIG_TYPE} CONFIG_NAME_UPPER)
    if(${CONFIG_TYPE} STREQUAL "Debug")
        target_compile_definitions(Game PRIVATE RK_DEBUG)
    elseif(${CONFIG_TYPE} STREQUAL "Release")
        target_compile_definitions(Game PRIVATE RK_RELEASE)
    elseif(${CONFIG_TYPE} STREQUAL "Dist")
        target_compile_definitions(Game PRIVATE RK_DIST)
    endif()
endforeach()
