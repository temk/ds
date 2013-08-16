cmake_minimum_required(VERSION 2.6)

project(ds-utils)


file(GLOB_RECURSE SOURCES src src/*.cpp)

include_directories(${ds-core_SOURCE_DIR}/include) 
link_directories(${ds-core_BINARY_DIR}) 

add_executable(ds2csv src/ds2csv.cpp)

target_link_libraries(ds2csv ds) 

install(TARGETS ds2csv  DESTINATION bin)
