#include <ds/utils.h>
#include <stdexcept>
#include <map>


using namespace ds;
using namespace std;

int
ds::size_of(type_t type) {
	switch(type) {
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
		
		default:
			return -1;
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

static const char * ENDIAN_NAME [] = { "invalid", "little", "big" };

static const char * TYPE_NAME [] = {
		"invalid", "bool", 
		"int8",   "int16",  "int32",  "int64", 
		"uint8", "uint16", "uint32", "uint64", 
		"float32", "float64", 
		"string8", "string16", "string32"
};

std::ostream &
ds::operator<<(std::ostream &out, type_t t) {
	return out << TYPE_NAME[static_cast<int>(t)];
}

std::ostream &
ds::operator<<(std::ostream &out, endian_t e) {
	return out << ENDIAN_NAME[static_cast<int>(e)];
}

std::istream &
ds::operator>>(std::istream &in, type_t &t) {
	static map<string, type_t> type_map;
	if (type_map.empty()) {
		for (int k = 0; k < sizeof(TYPE_NAME)/sizeof(TYPE_NAME[0]); ++ k) {
			type_map[ TYPE_NAME[k] ] = static_cast<type_t>(k);
		}
	}
	string val = "invalid";
	in >> val;
	t = type_map[val];
	return in;
}

std::istream &
ds::operator>>(std::istream &in, endian_t &e) {
	static map<string, endian_t> endian_map;
	if (endian_map.empty()) {
		for (int k = 0; k < sizeof(ENDIAN_NAME)/sizeof(ENDIAN_NAME[0]); ++ k) {
			endian_map[ ENDIAN_NAME[k] ] = static_cast<endian_t>(k);
		}
	}
	string val = "invalid";
	in >> val;
	e = endian_map[val];
	return in;
}
