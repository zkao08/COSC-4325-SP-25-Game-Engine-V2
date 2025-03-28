cmake_minimum_required(VERSION 3.10)
project(GameEngine)

set(CMAKE_CXX_STANDARD 17)

# Enable FetchContent if you want an alternative to submodules
include(FetchContent)

# Add subdirectories for each of the libraries
add_subdirectory(third_party_libs/openal-soft)
add_subdirectory(third_party_libs/bgfx)
add_subdirectory(third_party_libs/box2d)
add_subdirectory(third_party_libs/sdl)

# Specify the path to the ImGui source files (assuming they're located in third_party_libs/imgui)
set(IMGUI_DIR "third_party_libs/imgui")

# Add ImGui source files manually
file(GLOB IMGUI_SRC
    "${IMGUI_DIR}/imgui*.cpp"
)

# Create the main game engine executable
# Add .cpp files here to be included in the build
add_executable(GameEngine src/main.cpp ${IMGUI_SRC})

# Include directories for each library
target_include_directories(GameEngine PRIVATE
    third_party_libs/openal-soft/include
    third_party_libs/bgfx/include
    ${IMGUI_DIR}  # Include the ImGui directory
    third_party_libs/box2d
    third_party_libs/sdl/include
)

# Link libraries to your game engine executable
target_link_libraries(GameEngine PRIVATE 
    OpenAL
    bgfx
    imgui  # imgui will be included directly as source files, so this isn't necessary anymore
    box2d
    SDL2
)

# If you need to link OpenGL (for bgfx)
#find_package(OpenGL REQUIRED)
#target_link_libraries(GameEngine PRIVATE OpenGL::GL)
