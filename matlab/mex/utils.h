#ifndef __DS_MEX_UTILS_H__
#define __DS_MEX_UTILS_H__

#include <ds/types.h>
#include <mex.h>

namespace ds {
	class storage;
	class string_accessor;
	
	union ds_handle {
		uint64_t h; 
		storage *s; 
	};
	
	endian_t str_to_endian(const char *s);
	int str_to_mode(const char *s);
	type_t str_to_type(const char *s);
	
	const char *endian_to_str(endian_t);
	const char *type_to_str(type_t);
	
	mxClassID type_to_class(type_t);
	
	string_accessor * get_string_accessor(type_t type);
}


#endif //__DS_MEX_UTILS_H__
