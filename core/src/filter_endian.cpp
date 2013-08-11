#include <ds/filter_endian.h>

#include <stdexcept>
#include <algorithm>

#if defined(__GNUC__)
#define GCC_VERSION (__GNUC__ * 10000  + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)
#	if GCC_VERSION > 40700
#		define DS_SWAP16	__builtin_bswap16
#		define DS_SWAP32	__builtin_bswap32
#		define DS_SWAP64	__builtin_bswap64
#	else
#		define DS_SWAP16	ds_swap16
#		define DS_SWAP32    ds_swap32
#		define DS_SWAP64    ds_swap64
#	endif // GCC_VERSION > 40700
#elif defined(WIN32) 
#	define DS_SWAP16	_byteswap_ushort
#	define DS_SWAP32	_byteswap_ulong
#	define DS_SWAP64	_byteswap_uint64
#else 
#	error("Unknown compiler")
#endif // compiler

using namespace ds;
using namespace std;

inline uint16_t ds_swap16(uint16_t x) {
	return ((x & 0x00FF) << 8) | 
	       ((x & 0xFF00) >> 8);
}

inline uint32_t ds_swap32(uint32_t x) {
	return ((x & 0x000000FF) << 24) | 
	       ((x & 0x0000FF00) << 8)  | 
	       ((x & 0x00FF0000) >> 8)  | 
	       ((x & 0xFF000000) >> 24);
}

inline uint64_t ds_swap64(uint64_t x) {
	return ((x & 0x00000000000000FFL) << 56) | 
	       ((x & 0x000000000000FF00L) << 40) | 
	       ((x & 0x0000000000FF0000L) << 24) | 
	       ((x & 0x00000000FF000000L) <<  8) | 
	       ((x & 0x000000000000FF00L) >>  8) | 
	       ((x & 0x0000000000FF0000L) >> 24) | 
	       ((x & 0x00000000FF000000L) >> 40) | 
	       ((x & 0x00000000FF000000L) >> 56);
}

static void convert16(const void *src, void *dst, size_t num);
static void convert32(const void *src, void *dst, size_t num);
static void convert64(const void *src, void *dst, size_t num);

filter_endian::filter_endian(const error_handler &ref, type_t type, size_t siz, size_t cap) 
 : filter(ref, type, siz, type, siz), cap_(cap) {
	 
	buff_ = new char[siz * cap_];
	
	switch(siz) {
	case 2:
		conv_ = &convert16;
		break;
		
	case 4:
		conv_ = &convert32;
		break;
		
	case 8:
		conv_ = &convert64;
		break;
		
	default:
		throw runtime_error("Unexpected element size");
	}
}

filter_endian::~filter_endian() {
	delete [] buff_;
}
		
void 
filter_endian::put(const void *data, size_t num) {
	const char * src = static_cast<const char *>(data);
	
	while(num > 0) {
		size_t chunk = min(num, cap_);
		conv_(src, buff_, chunk);
		next() ->put(buff_, chunk);
		src += chunk;
		num -= chunk;
	}
}

void 
filter_endian::get(size_t offs, size_t num, void *data) {
	char * dst = static_cast<char *>(data);
	
	while(num > 0) {
		size_t chunk = min(num, cap_);
		next() ->get(offs, chunk, buff_);
		conv_(buff_, dst, chunk);
		dst += chunk;
		num -= chunk;
		offs += chunk * size_in();
	}	
}

void 
filter_endian::get(const void *indexes, int idx_siz, size_t num, void *data) {
	char * dst = static_cast<char *>(data);
	const char *idx = static_cast<const char *>(indexes);
	
	while(num > 0) {
		size_t chunk = min(num, cap_);
		next() ->get(idx, idx_siz, chunk, buff_);
		conv_(buff_, dst, chunk);
		dst += chunk;
		num -= chunk;
		idx += chunk * idx_siz;
	}	
}

// =====================================
static void convert16(const void *src, void *dst, size_t num) {
	const uint16_t * in = static_cast<const uint16_t *>(src);
	uint16_t * out = static_cast<uint16_t *>(dst);
	
	for (size_t k = 0; k < num; ++ k) {
		out[k] = DS_SWAP16(in[k]);
	}
}


static void convert32(const void *src, void *dst, size_t num) {
	const uint32_t * in = static_cast<const uint32_t *>(src);
	uint32_t * out = static_cast<uint32_t *>(dst);
	
	for (size_t k = 0; k < num; ++ k) {
		out[k] = DS_SWAP32(in[k]);
	}
}

static void convert64(const void *src, void *dst, size_t num) {
	const uint64_t * in = static_cast<const uint64_t *>(src);
	uint64_t * out = static_cast<uint64_t *>(dst);
	
	for (size_t k = 0; k < num; ++ k) {
		out[k] = DS_SWAP64(in[k]);
	}
}
