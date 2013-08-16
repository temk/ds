cmake_minimum_required(VERSION 2.6)

project(ds-example-cpp)


file(GLOB_RECURSE SOURCES src src/*.cpp)

include_directories(${ds-core_SOURCE_DIR}/include) 
link_directories(${ds-core_BINARY_DIR}) 

add_executable(ds-example-cpp ${SOURCES})

target_link_libraries(ds-example-cpp ds) 


