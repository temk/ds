#ifndef __DS_TYPES_H__
#define __DS_TYPES_H__
#include <stdint.h>
#include <unistd.h>

#include <iostream>
using namespace std;

namespace ds {
	typedef bool   bool_t;
	/*
	typedef char   int8_t;
	typedef short  int16_t;
	typedef int    int32_t;
	typedef long   int64_t;

	typedef unsigned char   uint8_t;
	typedef unsigned short  uint16_t;
	typedef unsigned int    uint32_t;
	typedef unsigned long   uint64_t;
	*/
	typedef float  float32_t;
	typedef double float64_t;
	
	typedef unsigned char  * str8_t;
	typedef unsigned short * str16_t;
	typedef unsigned int   * str32_t;
	
	typedef char    * blob_t;
	
	enum type_t {
		DS_T_INVALID = 0,
		
		DS_T_BOOL,
		
		DS_T_INT8,
		DS_T_INT16,
		DS_T_INT32,
		DS_T_INT64,
		
		DS_T_UINT8,
		DS_T_UINT16,
		DS_T_UINT32,
		DS_T_UINT64,
		
		DS_T_FLOAT32,
		DS_T_FLOAT64,
		
		DS_T_STRING8,
		DS_T_STRING16,
		DS_T_STRING32,
		
		DS_T_BLOB
	};
	
	static const int DS_ENDIAN_LITTLE = 0x0;
	static const int DS_ENDIAN_BIG    = 0x1;
	
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__	
#	define	DS_ENDIAN_HOST   DS_ENDIAN_LITTLE
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__	
#	define DS_ENDIAN_HOST    DS_ENDIAN_BIG
#else
#	error("ENDIANNESS")	
#endif 

};

#endif // __DS_TYPES_H__
