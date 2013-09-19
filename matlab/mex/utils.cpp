#include "utils.h"
#include "mx_string_accessor.h"

#include <ds/utils.h>
#include <ds/storage.h>
#include <ds/lookup.h>

#include <stdexcept>
#include <string.h>

using namespace ds;
using namespace std;

endian_t 
ds::str_to_endian(const char *s) {
	if (!strcmp(s, "big")) {		
		return DS_E_BIG;
	} else if (!strcmp(s, "little")) {
		return DS_E_LITTLE;
	} else if (!strcmp(s, "host")) {
		return DS_E_HOST;
	} else {
		throw runtime_error("unexpected endian (string) value");
	}
}

const char *
ds::endian_to_str(endian_t e) {
	switch(e) {
		case DS_E_BIG:    return "big";
		case DS_E_LITTLE: return "little";
		default:
				throw runtime_error("unexpected endian (int) value");
	}
}


type_t
ds::str_to_type(const char *s) {
	if (!strcmp(s, "logical")) {		
		return DS_T_BOOL;
	} else if (!strcmp(s, "int8")) {
		return DS_T_INT8;
	} else if (!strcmp(s, "int16")) {
		return DS_T_INT16;
	} else if (!strcmp(s, "int32")) {
		return DS_T_INT32;
	} else if (!strcmp(s, "int64")) {
		return DS_T_INT64;
	} else if (!strcmp(s, "uint8")) {
		return DS_T_UINT8;
	} else if (!strcmp(s, "uint16")) {
		return DS_T_UINT16;
	} else if (!strcmp(s, "uint32")) {
		return DS_T_UINT32;
	} else if (!strcmp(s, "uint64")) {
		return DS_T_UINT64;
	} else if (!strcmp(s, "single")) {
		return DS_T_FLOAT32;
	} else if (!strcmp(s, "double")) {
		return DS_T_FLOAT64;
	} else if (!strcmp(s, "char8")) {
		return DS_T_STRING8;
	} else if (!strcmp(s, "char")) {
		return DS_T_STRING16;
	} else if (!strcmp(s, "char32")) {
		return DS_T_STRING32;
	} else {
		cout << "unexpected type (string) value: " << s << endl;
		throw runtime_error("unexpected type (string) value");		
	}
}

const char *
ds::type_to_str(type_t t) {
	switch(t) {
		case DS_T_BOOL:     return "logical";
		case DS_T_INT8:     return "int8";
		case DS_T_INT16:    return "int16";
		case DS_T_INT32:    return "int32";
		case DS_T_INT64:    return "int64";
		case DS_T_UINT8:    return "uint8";
		case DS_T_UINT16:   return "uint16";
		case DS_T_UINT32:   return "uint32";
		case DS_T_UINT64:   return "uint64";
		case DS_T_FLOAT32:  return "single";
		case DS_T_FLOAT64:  return "double";
		case DS_T_STRING8:  return "char8";
		case DS_T_STRING16: return "char";
		case DS_T_STRING32: return "char32";
		default:
			throw runtime_error("unexpected type (int) value");		
	}
}

mxClassID
ds::type_to_class(type_t t) {
	switch(t) {
		case DS_T_BOOL:     return mxINT8_CLASS;
		case DS_T_INT8:     return mxINT8_CLASS;
		case DS_T_INT16:    return mxINT16_CLASS;
		case DS_T_INT32:    return mxINT32_CLASS;
		case DS_T_INT64:    return mxINT64_CLASS;
		case DS_T_UINT8:    return mxUINT8_CLASS;
		case DS_T_UINT16:   return mxUINT16_CLASS;
		case DS_T_UINT32:   return mxUINT32_CLASS;
		case DS_T_UINT64:   return mxUINT64_CLASS;
		case DS_T_FLOAT32:  return mxSINGLE_CLASS;
		case DS_T_FLOAT64:  return mxDOUBLE_CLASS;
		case DS_T_STRING8:  return mxCHAR_CLASS;
		case DS_T_STRING16: return mxCHAR_CLASS;
		case DS_T_STRING32: return mxCHAR_CLASS;
		default:
			throw runtime_error("unexpected type (int) value");		
	}
}

int 
ds::str_to_mode(const char *s) {
	int mode = 0;
	for (; *s; ++ s) {
		switch(*s) {
			case 'r':
				mode |= DS_O_READ;
				break;
				
			case 'w':
				mode |= DS_O_WRITE;
				break;
				
			case 'c':
				mode |= DS_O_CREATE;
				break;
				
			case 't':
				mode |= DS_O_TRUNC;
				break;
				
			default:
				throw runtime_error("unexcepted storage open mode");
		}
	}
	
	return mode;
}


string_accessor * 
ds::get_string_accessor(type_t type) {
	static string_accessor * acc_8  = new mx_string_accessor<char>();
	static string_accessor * acc_16 = new mx_string_accessor<short>();
	static string_accessor * acc_32 = new mx_string_accessor<int>();
	
	switch(type) {
		case DS_T_STRING8:  return acc_8;
		case DS_T_STRING16: return acc_16;
		case DS_T_STRING32: return acc_32;
		default: return NULL;
	}
}


