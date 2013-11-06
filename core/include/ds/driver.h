#ifndef __DS_DRIVER_H__
#define __DS_DRIVER_H__

#include <ds/types.h>
#include <ds/error.h>

#include <string>
using namespace std;

namespace ds {
	class lookup;
	class column;
	class storage;
	
	class  driver : public error_handler {
	public:
		virtual ~driver() = 0;
		
		virtual void open(const string &base, int mode) = 0;	
		virtual void close() = 0;
		
		virtual void write(const string &key, const void *data, size_t len) = 0;
		
		virtual void read(const string &key, size_t offs, void *dst, size_t len) = 0;
		virtual void read(const string &key, const void *indexes, int idx_siz, size_t num, void *dst, size_t el_siz) = 0;
		
		virtual void truncate(const string &key, size_t new_len) = 0;

		virtual void remove(const column &) = 0;
		
		virtual void write_dictionary(const lookup &) = 0;
		virtual void write_index(const storage &) = 0;
		virtual void write_index(const column &) = 0;

		virtual void read_dictionary(lookup &) = 0;
		virtual void read_index(storage &) = 0;
		virtual void read_index(column &)  = 0;

        virtual int get_mode() const = 0;
		
	};
}


#endif // __DS_DRIVER_H__
