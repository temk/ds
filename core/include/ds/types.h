#ifndef __DS_TYPES_H__
#define __DS_TYPES_H__
#include <stdint.h>
#include <unistd.h>

#include <iostream>

namespace ds {
	typedef bool   bool_t;
	typedef float  float32_t;
	typedef double float64_t;

	typedef unsigned char  * str8_t;
	typedef unsigned short * str16_t;
	typedef unsigned int   * str32_t;

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
		DS_T_STRING32
	};

	enum endian_t {
		DS_E_INVALID = 0x0,
		DS_E_LITTLE  = 0x1,
		DS_E_BIG     = 0x2
	};

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#	define	DS_E_HOST   DS_E_LITTLE
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#	define DS_E_HOST    DS_E_BIG
#else
#	error("ENDIANNESS")
#endif
};

typedef ds::type_t ds_type_t;
#endif // __DS_TYPES_H__
