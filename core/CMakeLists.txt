cmake_minimum_required(VERSION 2.6)

project(ds-core)

set(LIB_NAME ds)

file(GLOB_RECURSE SOURCES src *.cpp)
file(GLOB_RECURSE HEADERS include *.h)

set(EXT_HEADER include/ds/types.h
               include/ds/utils.h
               include/ds/error.h
               include/ds/column.h
               include/ds/strings.h
               include/ds/storage.h)

include_directories(include)

if (DS_BUILD_CORE)
	add_library(${LIB_NAME}  SHARED ${SOURCES})
	add_library(${LIB_NAME}s STATIC ${SOURCES})

	install(TARGETS ${LIB_NAME}  DESTINATION lib)
	install(TARGETS ${LIB_NAME}s DESTINATION lib)
	install(FILES ${EXT_HEADER} DESTINATION include/ds)
	if (DS_BUILD_DOCS)
		find_package(Doxygen REQUIRED)
		if(DOXYGEN_FOUND)
			configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
			add_custom_target(run ALL ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
					         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                                                 COMMENT "Generating API documentation with Doxygen" VERBATIM)
			install(FILES ${CMAKE_CURRENT_BINARY_DIR}/cpp DESTINATION doc)
		endif(DOXYGEN_FOUND)
	endif (DS_BUILD_DOCS)
endif(DS_BUILD_CORE)

