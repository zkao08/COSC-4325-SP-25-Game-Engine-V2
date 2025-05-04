file(GLOB_RECURSE CLASS_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_library(components ${CLASS_FILES})
target_include_directories(components PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

include_directories(${CMAKE_SOURCE_DIR}/third_party_libs/imgui)
include_directories(${CMAKE_SOURCE_DIR}/third_party_libs/imgui/backends)
include_directories(${CMAKE_SOURCE_DIR}/third_party_libs/imgui/misc/cpp)
include_directories(${CMAKE_SOURCE_DIR}/third_party_libs/sol)
include_directories(${CMAKE_SOURCE_DIR}/third_party_libs/lua/src)
include_directories(${CMAKE_SOURCE_DIR}/src/Core)
include_directories(${CMAKE_SOURCE_DIR}/src/UI)