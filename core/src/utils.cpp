#include <ds/utils.h>
#include <stdexcept>


using namespace ds;
using namespace std;

size_t 
ds::size_of(type_t type) {
	switch(type) {
		case DS_T_INVALID: return 0;
		
		case DS_T_BOOL: return sizeof(bool);
		
		case DS_T_INT8:  return sizeof(int8_t);
		case DS_T_INT16: return sizeof(int16_t);
		case DS_T_INT32: return sizeof(int32_t);
		case DS_T_INT64: return sizeof(int64_t);
		
		case DS_T_UINT8:  return sizeof(uint8_t);
		case DS_T_UINT16: return sizeof(uint16_t);
		case DS_T_UINT32: return sizeof(uint32_t);
		case DS_T_UINT64: return sizeof(uint64_t);
		
		
		case DS_T_FLOAT32:  return sizeof(float32_t);
		case DS_T_FLOAT64:  return sizeof(float64_t);
		
		case DS_T_STRING8:  return sizeof(char *);
		case DS_T_STRING16: return sizeof(short *);		
		case DS_T_STRING32: return sizeof(int *);		
		case DS_T_BLOB:    return 0;
		
		default:
			throw runtime_error("size_of: unknown type");
	}
	
	return 0;
}

bool 
ds::is_numeric(type_t type) {
	switch(type) {
		
	case DS_T_INT8:  
	case DS_T_INT16: 
	case DS_T_INT32: 
	case DS_T_INT64: 
	case DS_T_UINT8: 
	case DS_T_UINT16:
	case DS_T_UINT32:
	case DS_T_UINT64:
		return true;
	
	default:
		return false;
	}	
}

bool 
ds::is_str(type_t type) {
	switch(type) {
		case DS_T_STRING8:
		case DS_T_STRING16:
		case DS_T_STRING32:
			return true;
		
		default:
			return false;
	}	
}