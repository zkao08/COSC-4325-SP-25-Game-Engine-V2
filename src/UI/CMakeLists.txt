file(GLOB_RECURSE CLASS_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_library(ui ${CLASS_FILES})
target_include_directories(ui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

include_directories(${CMAKE_SOURCE_DIR}/third_party_libs/imgui)
include_directories(${CMAKE_SOURCE_DIR}/third_party_libs/imgui/backends)
include_directories(${CMAKE_SOURCE_DIR}/third_party_libs/imgui/misc/cpp)
include_directories(${CMAKE_SOURCE_DIR}/src/Core)