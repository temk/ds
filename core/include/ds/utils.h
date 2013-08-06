#ifndef __DS_UTILS_H__
#define __DS_UTILS_H__
#include <ds/types.h>

namespace ds {
	size_t size_of(type_t type);	
	bool is_numeric(type_t type);
	bool is_str(type_t type);
}

#endif // __DS_UTILS_H__
