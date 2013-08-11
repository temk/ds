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
		
		size_t filesize(const string &filename) const;
		void create_empty_index();
		
		template<typename T> void read_field(istream &in, const string &magic, T *val);
	public:
		driver_dir();
		~driver_dir();
		
		void open(const string &base, int mode);	
		void close();
		
		void write(const string &key, const void *data, size_t len);
		void read(const string &key, size_t offs, void *dst, size_t len);
		void read(const string &key, const void *indexes, int idx_siz, size_t num, void *dst, size_t el_siz);
		
		void truncate(const string &key, size_t sz);

		void remove(const column &);

		void write_dictionary(const lookup &);
		void write_index(const storage &);
		void write_index(const column &);

		void read_dictionary(lookup &);
		void read_index(storage &);
		void read_index(column &);		
	};
}
#endif // __DS_DRIVER_DIR_H__
