#ifndef __DS_UTILS_H__
#define __DS_UTILS_H__
#include <ds/types.h>

namespace ds {
	int size_of(type_t type);	
	bool is_numeric(type_t type);
	bool is_str(type_t type);
	
	std::ostream &operator<<(std::ostream &, type_t);
	std::ostream &operator<<(std::ostream &, endian_t);
	std::istream &operator>>(std::istream &, type_t &);
	std::istream &operator>>(std::istream &, endian_t &);	
	
	template<typename T> size_t 
	str_length(const T *str) {
		const T *p = str;
		for (;*str; ++ str);
		return (str - p);
	}

	template<typename T, typename U> void 
	str_copy(const T *src, U *dst, size_t len) {
		for(; len ; -- len)
			*dst ++ = *src ++;
	}	

	template<typename T, typename U> bool 
	str_less(const T *a, const U *b, size_t len) {
		for(; len > 0 && *a == *b; -- len, ++ a, ++ b);
		return (len == 0) || *a < *b;
	}	
	
	template<typename U, typename T>U *	
	str_dup(const T *t, size_t len) {
		U * u = new U[len + 1];
		str_copy<T, U>(t, u, len);
		u[len] = 0;
		return u;
	}	
}

#endif // __DS_UTILS_H__
