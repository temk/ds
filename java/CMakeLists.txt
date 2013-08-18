cmake_minimum_required(VERSION 2.6)

find_package(Java REQUIRED)
find_package(JNI REQUIRED)
include(UseJava)

project(dsjni)

file(GLOB_RECURSE SOURCES src src/main/native/src/*.cpp)
file(GLOB_RECURSE HEADERS include src/main/native/include/*.h)

include_directories(${JNI_INCLUDE_DIRS} 
                    ${ds-core_SOURCE_DIR}/include
                    src/main/native/include)


add_library(dsjni  SHARED ${SOURCES})

set(CMAKE_JAVA_TARGET_OUTPUT_NAME ds)
add_jar(dsjar src/main/java/org/temk/ds/*.java)

install(TARGETS dsjni DESTINATION lib)
install_jar(dsjar ${CMAKE_INSTALL_PREFIX}/lib/java)

