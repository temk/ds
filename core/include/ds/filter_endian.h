#ifndef __DS_FILTER_ENDIAN_H__
#define __DS_FILTER_ENDIAN_H__
#include <ds//filter.h>

namespace ds {
	
	class filter_endian : public filter {
	private:
		typedef void (*converter)(const void *src, void *dst, size_t num);
		
		size_t cap_;
		char * buff_;
		converter conv_;
		
	public:
		filter_endian(type_t type, size_t siz, size_t cap);
		~filter_endian();		
		
		void put(const void *data, size_t num);
		void get(size_t offs, size_t num, void *data);
		void get(const void *indexes, int idx_siz, size_t num, void *data);
	};		
	
}

#endif //__DS_FILTER_ENDIAN_H__
