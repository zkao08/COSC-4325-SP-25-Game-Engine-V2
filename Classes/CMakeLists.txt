file(GLOB_RECURSE CLASS_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_library(classes ${CLASS_FILES})
set_target_properties(classes PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(classes PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
#include_directories(${CMAKE_SOURCE_DIR}/src/Core)