#ifndef __DS_FILTER_STR_H__
#define __DS_FILTER_STR_H__
#include <ds/filter.h>

#include <string>
using namespace std;

namespace ds {	
	class lookup;
	class driver;
	class string_accessor;
	
	class filter_str : public filter {
	private:
		size_t cap_;
		char * buff_;
		lookup *lookup_;
		driver *driver_;
		
	public:	
		filter_str(const error_handler &ref, type_t t_in, type_t t_out, size_t cap, const string &key, driver * drv, bool read_dict);
		~filter_str();		
		
		void flush();
		
		void put(const void *data, size_t num);
		void get(size_t offs, size_t num, void *data);
		void get(const void *indexes, int idx_siz, size_t num, void *data);
		
		void set_string_accessor(const string_accessor *);
		
	};		
}

#endif //__DS_FILTER_STR_H__
