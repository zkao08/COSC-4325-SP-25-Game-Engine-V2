cmake_minimum_required(VERSION 3.10)
project(GameEngine)
set(CMAKE_CXX_STANDARD 17)

# Find Lua libraries
set(LUA_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/third_party_libs/lua/src)
file(GLOB LUA_SOURCES ${LUA_SRC_DIR}/*.c)

# Remove the standalone executables from the build
list(REMOVE_ITEM LUA_SOURCES 
    ${LUA_SRC_DIR}/lua.c    # The Lua interpreter
    ${LUA_SRC_DIR}/luac.c   # The Lua compiler
)

# Create a Lua static library
add_library(lua STATIC ${LUA_SOURCES})
target_include_directories(lua PUBLIC ${LUA_SRC_DIR})

# Add subdirectories for each of the libraries
add_subdirectory(third_party_libs/box2d)
add_subdirectory(third_party_libs/raylib)
add_subdirectory(third_party_libs/imgui)
add_subdirectory(src/Core)
add_subdirectory(src/UI)
add_subdirectory(Classes)

# Sol2 is header-only, so just include its directory
set(SOL2_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/third_party_libs/sol")

# Specify the path to the ImGui source files
set(IMGUI_DIR "third_party_libs/imgui")

# Add ImGui source files manually
file(GLOB IMGUI_SRC
    "${IMGUI_DIR}/imgui*.cpp"
)

# Specify the source directory
set(SRC_DIR "src")

# Gather all .cpp and .h files
file(GLOB_RECURSE GAME_SRC "${SRC_DIR}/*.cpp")
file(GLOB_RECURSE GAME_HEADERS "${SRC_DIR}/*.h")

# Remove test files from the main game engine build
# This prevents test files from being included in the main game executable
list(FILTER GAME_SRC EXCLUDE REGEX ".*_test\\.cpp$")

# Group ImGui files under "ThirdParty/ImGui" in Visual Studio
source_group("ThirdParty/ImGui" FILES ${IMGUI_SRC})

# Group .cpp files under "Source Files"
source_group("SourceFiles" FILES ${GAME_SRC})

# Group .h files under "Header Files"
source_group("HeaderFiles" FILES ${GAME_HEADERS})

# -----------------------------
# Dynamically Find XAudio2
# -----------------------------
# Look for the XAudio2 include directory
file(GLOB XAUDIO2_INCLUDE_DIRS "$ENV{WindowsSdkDir}/Include/*/um")

# Ensure we have found the include directory
if(XAUDIO2_INCLUDE_DIRS)
    list(SORT XAUDIO2_INCLUDE_DIRS)
    list(GET XAUDIO2_INCLUDE_DIRS 0 XAUDIO2_INCLUDE_DIR)
    message(STATUS "Found XAudio2 include directory: ${XAUDIO2_INCLUDE_DIR}")
else()
    message(FATAL_ERROR "XAudio2 include directory not found. Please ensure the Windows SDK is installed.")
endif()

# Look for the XAudio2 library
file(GLOB XAUDIO2_LIBRARIES "$ENV{WindowsSdkDir}/Lib/*/um/x64/xaudio2_8.lib" "$ENV{WindowsSdkDir}/Lib/*/um/x64/xaudio2.lib")

# Ensure we have found the library
if(XAUDIO2_LIBRARIES)
    list(SORT XAUDIO2_LIBRARIES)
    list(GET XAUDIO2_LIBRARIES 0 XAUDIO2_LIBRARY)
    message(STATUS "Found XAudio2 library: ${XAUDIO2_LIBRARY}")
else()
    message(FATAL_ERROR "XAudio2 library not found. Please ensure the Windows SDK is installed.")
endif()

# Create the main game engine executable
add_executable(GameEngine ${GAME_SRC} ${IMGUI_SRC})

# Add header files to the target, just for organizing in Visual Studio
target_sources(GameEngine PRIVATE ${GAME_HEADERS})

# Include directories for the main game engine
target_include_directories(GameEngine PRIVATE
    ${IMGUI_DIR}
    third_party_libs/box2d
    ${SOL2_INCLUDE_DIR}
    ${LUA_SRC_DIR} 
    ${SRC_DIR} 
    ${XAUDIO2_INCLUDE_DIR}
	third_party_libs/raylib/src
)

# Link libraries to your game engine executable
target_link_libraries(GameEngine PRIVATE 
	classes
	core
	ui
    box2d
	d3d11
	imgui
    lua
    ${XAUDIO2_LIBRARY}
	raylib
)

# -------------------------------
# Test Executables
# -------------------------------

# Audio Test Executable
#add_executable(AudioTest 
#    src/audio_test.cpp
#    src/AudioManager.cpp
#    src/AudioLuaAPI.cpp
#    src/SoundResource.cpp
#)

# Include directories for AudioTest
#target_include_directories(AudioTest PRIVATE
#    ${IMGUI_DIR}
#    third_party_libs/box2d
#    ${SOL2_INCLUDE_DIR}
#    ${LUA_SRC_DIR} 
#    ${SRC_DIR} 
#    ${XAUDIO2_INCLUDE_DIR}
#)

# Link libraries to AudioTest
#target_link_libraries(AudioTest PRIVATE 
#    lua
#    ${XAUDIO2_LIBRARY}
#	raylib
#)

# You can add more test executables here following the same pattern
# For example:

# Physics Test Executable
# add_executable(PhysicsTest 
#     src/physics_test.cpp
#     src/Physics/PhysicsSystem.cpp
#     # Add other required source files
# )
# 
# target_include_directories(PhysicsTest PRIVATE
#     ${SRC_DIR}
#     third_party_libs/box2d
# )
# 
# target_link_libraries(PhysicsTest PRIVATE
#     box2d
# )

# Rendering Test Executable 
# add_executable(RenderingTest 
#     src/rendering_test.cpp
#     src/Rendering/RenderSystem.cpp
#     # Add other required source files
# )
# 
# target_include_directories(RenderingTest PRIVATE
#     ${IMGUI_DIR}
#     ${SRC_DIR}
# )
# 
# target_link_libraries(RenderingTest PRIVATE
#     # Add required libraries
# )