#ifndef __DS_COLUMN_H__
#define __DS_COLUMN_H__
#include <ds/types.h>

#include <string>
#include <iostream>

using namespace std;

namespace ds {
	class filter;
	class driver;
	class column;
	class storage;
	
	ostream& operator<<(ostream &, const column &);
	istream& operator>>(istream &, column &);
	
	class column
	{
	private:
		int siz_;
		int endian_;
		size_t len_;
		bool dirty_;
		string name_;
		type_t type_;

		driver *driver_;
		filter *filter_;
		storage &storage_;
		
		void push_filter(filter *);
		void init_filters();
		
		void read_index();
		void write_index() const;
	public:
		column(storage &s, string name);
		column(storage &s, type_t type, const string &name, int endian, int siz);
		
		virtual ~column();
		
		void remove(bool leave_on_disk = false);
		
		column &flush();
		
		column &truncate(size_t len = 0);
		
		column &append(const void *data, size_t data_num);

		int read(size_t offset, size_t num, void *dst);
		int read(const void *indexes, int idx_siz, size_t num, void *dst);
		
		template<typename T> int read(const T* indexes, size_t num, void *dst);

		size_t index() const;
		
		inline const string & name() const;
		
		inline size_t length() const;

		inline int element_size() const;
		inline int endian() const;
		inline type_t type() const;
	};
	
	// =============================================================================
	inline type_t column::type() const {
		return type_;
	}
	
	inline int column::endian() const {
		return endian_;
	}
	
	inline int column::element_size() const {
		return siz_;
	}
	
	inline const string & column::name() const {
		return name_;
	}
	
	inline size_t column::length() const {
		return len_;
	}
	
	template<typename T> int 
	column::read(const T* indexes, size_t num, void *dst) {
		return read(indexes, sizeof(T), num, dst);
	}	
}
#endif // __DS_COLUMN_H__
