#ifndef __DS_DRIVER_H__
#define __DS_DRIVER_H__

#include <string>

using namespace std;

namespace ds {
	class  driver {
	public:
		virtual ~driver() = 0;
		virtual void open(const string &base, int mode) = 0;	
		virtual void close() = 0;
		
		virtual void write(const string &key, const void *, size_t len, bool append = true) = 0;
		virtual void read(const string &key, size_t offs, void *dst, size_t len) = 0;
		virtual void read(const string &key, const void *indexes, int idx_siz, size_t num, void *dst, size_t el_siz) = 0;
		
//		virtual void write_index(const string &) = 0;
//		virtual void read_index(string &) = 0;
		
		virtual size_t length(const string &key) = 0;
		virtual void truncate(const string &key, size_t new_len) = 0;
		
//		virtual bool has_index() = 0;
	};
}


#endif // __DS_DRIVER_H__
