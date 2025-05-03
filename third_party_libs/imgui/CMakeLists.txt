file(GLOB_RECURSE CLASS_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_library(imgui ${CLASS_FILES})
target_include_directories(imgui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(imgui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/backends)
target_include_directories(imgui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/misc/cpp)
