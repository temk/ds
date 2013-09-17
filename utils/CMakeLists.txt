cmake_minimum_required(VERSION 2.6)

project(ds-utils)


file(GLOB_RECURSE SOURCES src src/*.cpp)

include_directories(${ds-core_SOURCE_DIR}/include)
include_directories(${ds-utils_SOURCE_DIR}/include)
link_directories(${ds-core_BINARY_DIR})

if (DS_BUILD_CORE)
	if (DS_BUILD_UTILS)
		add_executable(ds2csv src/ds2csv.cpp)
                add_executable(csv2ds src/csv2ds.cpp src/csv2ds_options.cpp src/appender.cpp)

		target_link_libraries(ds2csv ds) 
		target_link_libraries(csv2ds ds) 

		install(TARGETS ds2csv  DESTINATION bin)
		install(TARGETS csv2ds  DESTINATION bin)
	endif(DS_BUILD_UTILS)
endif(DS_BUILD_CORE)

