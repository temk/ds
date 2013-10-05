#ifndef __DS_STORAGE_H__
#define __DS_STORAGE_H__
#include <ds/types.h>
#include <ds/error.h>
#include <ds/meta.h>

#include <map>
#include <vector>
#include <string>

using namespace std;

namespace ds {
	class column;
	class driver;
	
	/**
	 * current version 
	**/
	static const int MAJOR_VERSION = 1;
	static const int MINOR_VERSION = 0;
	
	/**
	 * flags to open storage
	**/
	static const int DS_O_READ    = 0x01;
	static const int DS_O_WRITE   = 0x02;
	static const int DS_O_TRUNC   = 0x04;
	static const int DS_O_CREATE  = 0x08;
	static const int DS_O_DEFAULT = DS_O_READ | DS_O_WRITE | DS_O_CREATE;

	/**
	 * default buffer size 
	**/
	static const size_t DS_BUFF_SIZ = 1024;
	
	class storage : public error_handler
	{
	private:
		friend class column;
		
		typedef vector<column *> col_list_t;
		typedef map<string, column *> col_map_t;
		
		typedef void (*warn_handler_t)(const char *);
		
		size_t col_num_;
		size_t buff_siz_;
		
		col_map_t col_by_name_;
		col_list_t col_by_index_;
		
		driver *driver_;
		warn_handler_t warn_handler_;
		
		size_t index_of(const column *) const;
		
		void pop(column *);		
		void push(column *, ssize_t index = -1);
		
        meta meta_;
	public:
		storage();
		storage(const string &path, int mode = DS_O_DEFAULT, size_t buff_siz = DS_BUFF_SIZ);
		
		~storage();

		void open(const string &path, int mode = DS_O_DEFAULT, size_t buff_siz = DS_BUFF_SIZ);		
		void close();

		void flush();
		
		bool is_open() const;
		inline operator bool () const;

		inline size_t cols() const;
		
		const column & column_at(size_t idx) const;
		const column & column_at(const string &name) const;

		column & column_at(size_t idx);
		column & column_at(const string &name);
		
		/**
		 * add numeric column 
		**/
		column &add(type_t type, const string &name = "", endian_t = DS_E_HOST, ssize_t index = -1);
		
		/**
		 * add string column
		**/
		column &add(type_t type, type_t dict, const string &name = "", endian_t = DS_E_HOST, ssize_t index = -1);
		
		inline const column & operator[](size_t idx) const;
		inline const column & operator[](const char *name) const;
		inline const column & operator[](const string &name) const;

		inline column & operator[](size_t idx);
		inline column & operator[](const char *name);
		inline column & operator[](const string &name);		

        inline const meta &tags() const;
        inline meta &tags();
    };

	// ==============================================================================
	inline storage::operator bool () const {		
		return is_open();
	}

	inline size_t storage::cols() const {
		return col_num_;
	}

	inline const column & storage::operator[](size_t idx) const {
		return column_at(idx);
	}
	
	inline const column & storage::operator[](const char *name) const {		
		return column_at(name);
	}

	inline const column & storage::operator[](const string &name) const {		
		return column_at(name);
	}

	inline column & storage::operator[](size_t idx) {		
		return column_at(idx);
	}

	inline column & storage::operator[](const char *name) {		
		return column_at(name);
	}

	inline column & storage::operator[](const string &name) {		
		return column_at(name);
	}

    inline const meta &storage::tags() const {
        return meta_;
    }

    inline meta &storage::tags() {
        return meta_;
    }

}

// ==============================================================================================
extern "C" {
  void * ds_open(const char *path, int mode, int buff_siz);
  void * ds_add_column(void *ds, int type, int dict_type, const char *name, int endian, int index );
  void * ds_get_column_by_name(void *ds, const char *name);
  void * ds_get_column_by_index(void *ds, int index);
  void ds_add_meta(void *ds, const char *key, const char *val);
  void ds_flush(void *ds);
  void ds_close(void *ds);
}
// ==============================================================================================

#endif //__DS_STORAGE_H__
