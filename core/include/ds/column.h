#ifndef __DS_COLUMN_H__
#define __DS_COLUMN_H__
#include <ds/types.h>
#include <ds/error.h>
#include <ds/meta.h>


#include <string>
#include <iostream>

using namespace std;

namespace ds {
	class filter;
	class lookup;
	class driver;
	class column;
	class storage;
	class string_accessor;

	class column : public error_handler
	{
	private:
		bool dirty_;
		string name_;
		size_t width_;
		size_t length_;
		endian_t endian_;
		bool compressed_;

		type_t int_type_;
		type_t ext_type_;

		filter *filter_;
		storage &storage_;
        meta meta_;

	protected:
		void push_filter(filter *);
		virtual void init_filters();

	public:
        column(storage &s, string name);
        column(storage &s, type_t int_type, type_t ext_type, const string &name, size_t width, endian_t endian, bool compessed);

        void init(type_t int_type, type_t ext_type, size_t width, size_t length, endian_t endian, bool compessed);

		virtual ~column();

		void remove();
		void detach();

		column &flush();

		column &truncate(size_t len = 0);

		column &append(const void *data, size_t data_num);

		int read(size_t offset, size_t num, void *dst);
		int read(const void *indexes, int idx_siz, size_t num, void *dst);

		template<typename T> int read(const T* indexes, size_t num, void *dst);

		size_t index() const;

		inline const string & name() const;

        inline size_t width() const;
        inline size_t length() const;

		inline endian_t endian() const;
		inline type_t type() const;
		inline type_t ext_type() const;

        inline bool compressed() const;

        inline const meta &tags() const;
        inline meta &tags();

        void set_string_accessor(string_accessor *);
	};

	// =============================================================================
	inline type_t column::type() const {
		return int_type_;
	}

	inline bool column::compressed() const {
		return compressed_;
	}

	inline type_t column::ext_type() const {
		return ext_type_;
	}

	inline endian_t column::endian() const {
		return endian_;
	}

	inline const string & column::name() const {
		return name_;
	}

    inline size_t column::width() const {
        return width_;
    }

    inline size_t column::length() const {
        return length_;
    }

    inline const meta &column::tags() const {
        return meta_;
    }

    inline meta &column::tags() {
        return meta_;
    }

	template<typename T> int
	column::read(const T* indexes, size_t num, void *dst) {
		return read(indexes, sizeof(T), num, dst);
	}
}

// ==============================================================================================
extern "C" {
  void ds_column_append(void *col, void *, uint32_t len);
	const char * ds_column_name(const void *col);
	size_t ds_column_width(const void *col);
	size_t ds_column_length(const void *col);
  ds_type_t ds_column_type(const void *col);

	int ds_column_read(void *col, size_t offset, size_t num, void *dst);
	int ds_column_read_double(void *col, size_t offset, size_t num, double *dst); 

}

#endif // __DS_COLUMN_H__
