#ifndef __DS_DRIVER_DIR_H__
#define __DS_DRIVER_DIR_H__
#include <ds/driver.h>

using namespace std;

namespace ds {
	
	class driver_dir : public driver {
	private:
		int mode_;
		string base_;
		
		void construct_filename(const string &key, string &filename) const;
		void construct_fullpath(const string &path, string &fullpath) const;
		void remove_all() const;
		
	public:
		driver_dir();
		~driver_dir();
		
		void open(const string &base, int mode);	
		void close();
		
		void write(const string &key, const void *, size_t len, bool append = true);
		void read(const string &key, size_t offs, void *dst, size_t len);
		void read(const string &key, const void *indexes, int idx_siz, size_t num, void *dst, size_t el_siz);
		
//		void write_index(const string &);
//		void read_index(string &);	
		
		size_t length(const string &key);
		void truncate(const string &key, size_t sz);
		
//		bool has_index();
	};
}
#endif // __DS_DRIVER_DIR_H__
