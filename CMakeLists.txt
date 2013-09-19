cmake_minimum_required(VERSION 2.6)

include(cmake.config)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake-modules)

add_subdirectory(core)
add_subdirectory(utils)
add_subdirectory(examples/cpp)
add_subdirectory(java)
add_subdirectory(matlab)



