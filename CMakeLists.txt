cmake_minimum_required(VERSION 3.10)
project(GameEngine)

set(CMAKE_CXX_STANDARD 17)

# Enable FetchContent if you want an alternative to submodules
include(FetchContent)

# Add subdirectories for each of the libraries
add_subdirectory(third_party_libs/openal-soft)
add_subdirectory(third_party_libs/bgfx)
add_subdirectory(third_party_libs/box2d)
add_subdirectory(third_party_libs/sdl EXCLUDE_FROM_ALL)

# Specify the path to the ImGui source files (assuming they're located in third_party_libs/imgui)
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

# Group ImGui files under "ThirdParty/ImGui" in Visual Studio
source_group("ThirdParty/ImGui" FILES ${IMGUI_SRC})

# Group .cpp files under "Source Files"
source_group("SourceFiles" FILES ${GAME_SRC})

# Group .h files under "Header Files"
source_group("HeaderFiles" FILES ${GAME_HEADERS})

# Create the main game engine executable
add_executable(GameEngine ${GAME_SRC} ${IMGUI_SRC})

# Add header files to the target, just for organizing in Visual Studio (not for compilation)
target_sources(GameEngine PRIVATE ${GAME_HEADERS})

# -----------------------------
# Dynamically Find XAudio2
# -----------------------------
# Look for the XAudio2 include directory
#file(GLOB XAUDIO2_INCLUDE_DIRS "$ENV{WindowsSdkDir}/Include/*/um")

# Ensure we have found the include directory
#if(XAUDIO2_INCLUDE_DIRS)
    #list(SORT XAUDIO2_INCLUDE_DIRS)
    #list(GET XAUDIO2_INCLUDE_DIRS 0 XAUDIO2_INCLUDE_DIR)
    #message(STATUS "Found XAudio2 include directory: ${XAUDIO2_INCLUDE_DIR}")
#else()
    #message(FATAL_ERROR "XAudio2 include directory not found. Please ensure the Windows SDK is installed.")
#endif()

# Look for the XAudio2 library
#file(GLOB XAUDIO2_LIBRARIES "$ENV{WindowsSdkDir}/Lib/*/um/x64/xaudio2_8.lib" "$ENV{WindowsSdkDir}/Lib/*/um/x64/xaudio2.lib")

# Ensure we have found the library
#if(XAUDIO2_LIBRARIES)
    #list(SORT XAUDIO2_LIBRARIES)
    #list(GET XAUDIO2_LIBRARIES 0 XAUDIO2_LIBRARY)
    #message(STATUS "Found XAudio2 library: ${XAUDIO2_LIBRARY}")
#else()
    #message(FATAL_ERROR "XAudio2 library not found. Please ensure the Windows SDK is installed.")
#endif()

# Link XAudio2 to the project
#target_include_directories(GameEngine PRIVATE ${XAUDIO2_INCLUDE_DIR})
#target_link_libraries(GameEngine PRIVATE ${XAUDIO2_LIBRARY})

# Include directories for each library
target_include_directories(GameEngine PRIVATE
    third_party_libs/openal-soft/include
    third_party_libs/bgfx/include
    ${IMGUI_DIR}  # Include the ImGui directory
    third_party_libs/box2d
    third_party_libs/sdl/include  # SDL2 include path
    ${SRC_DIR}  # Include the src directory
)

# Link libraries to your game engine executable
target_link_libraries(GameEngine PRIVATE 
    OpenAL
    bgfx
    box2d
    SDL3::SDL3
)

# If you need to link OpenGL (for bgfx)
#find_package(OpenGL REQUIRED)
#target_link_libraries(GameEngine PRIVATE OpenGL::GL)