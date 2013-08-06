cmake_minimum_required(VERSION 2.6)

project(ds-core)

set(LIB_NAME ds)

file(GLOB_RECURSE SOURCES src *.cpp)
file(GLOB_RECURSE HEADERS include *.h)

include_directories(include)

add_library(${LIB_NAME}  SHARED ${SOURCES})
add_library(${LIB_NAME}s STATIC ${SOURCES})

set(CMAKE_INSTALL_PREFIX ..)
install(TARGETS ${LIB_NAME}  DESTINATION lib)
install(TARGETS ${LIB_NAME}s DESTINATION lib)


