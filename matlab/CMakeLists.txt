cmake_minimum_required(VERSION 2.6)

project(dsmex)


if (DS_BUILD_MATLAB)
	include(DSUseMex)
	include_directories(${ds-core_SOURCE_DIR}/include
                		 src/main/native/include)

	set(targets ds_add ds_close ds_flush ds_info ds_open ds_read ds_read_index ds_remove ds_trunc ds_write)
	set(mat_files DataStorage.m)

	foreach(target ${targets})
		add_mex_target(${target} mex/${target}.cpp mex/utils.cpp)
		target_link_libraries(${target} ds) 
	endforeach(target)

	install(TARGETS ${targets} DESTINATION ${CMAKE_INSTALL_PREFIX}/matlab/mex)
        install(FILES ${mat_files} DESTINATION ${CMAKE_INSTALL_PREFIX}/matlab)
endif(DS_BUILD_MATLAB)


