#ifndef __DS_MEX_UTILS_H__
#define __DS_MEX_UTILS_H__

#include <ds/types.h>
#include <mex.h>

namespace ds {
	class storage;
	union ds_handle {
		uint64_t h; 
		storage *s; 
	};
	
	int str_to_endian(const char *s);
	int str_to_mode(const char *s);
	type_t str_to_type(const char *s);
	
	const char *endian_to_str(int);
	const char *type_to_str(type_t);
	
	mxClassID type_to_class(type_t);

	template<typename T> size_t str_length(const T *str) {
		const T *p = str;
		for (;*str; ++ str);
		return (str - p);
	}

	template<typename T, typename U> size_t str_copy(const T *src, U *dst, size_t len, bool null_term = true) {
		for(; len ; -- len)
			*dst ++ = *src ++;
			
		if (null_term)
			*dst = 0;
	}	
}


#endif //__DS_MEX_UTILS_H__
