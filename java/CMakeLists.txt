cmake_minimum_required(VERSION 2.6)

project(dsjni)

if (DS_BUILD_CORE)
    if (DS_BUILD_JAVA)
        find_package(Java REQUIRED)
        include(UseJava)


        file(GLOB_RECURSE JAVA_SOURCES *.java)

        set(JNI_INCLUDE_DIRS ${JAVA_ROOT}/include
        ${JAVA_ROOT}/include/linux)

        file(GLOB_RECURSE SOURCES src src/main/native/src/*.cpp)
        file(GLOB_RECURSE HEADERS include src/main/native/include/*.h)

        include_directories(${JNI_INCLUDE_DIRS} ${ds-core_SOURCE_DIR}/include src/main/native/include)

        add_library(dsjni  SHARED ${SOURCES})

        link_directories(${ds-core_BINARY_DIR})
        target_link_libraries(dsjni ds)

        set(CMAKE_JAVA_TARGET_OUTPUT_NAME ds)
        add_jar(java ${JAVA_SOURCES})

        install(TARGETS dsjni DESTINATION lib)
        install(FILES src/main/native/include/ds/jni/strings.h DESTINATION include/ds/jni)

        install_jar(java ${CMAKE_INSTALL_PREFIX}/lib/java)

        if (DS_BUILD_DOCS)
            create_javadoc(java FILES ${JAVA_SOURCES} INSTALLPATH doc)
        endif(DS_BUILD_DOCS)
    endif(DS_BUILD_JAVA)
endif(DS_BUILD_CORE)


