#ifndef __DS_FILTER_STR_H__
#define __DS_FILTER_STR_H__
#include <ds/filter.h>

namespace ds {	
	class lookup;
	class driver;
	
	class filter_str : public filter {
	private:
		size_t cap_;
		char * buff_;
		string name_;
		lookup *lookup_;
		driver *driver_;
		
	public:	
		filter_str(type_t t_in, type_t t_out, size_t cap, const string &name, driver *drv);
		~filter_str();		
		
		void flush();
		
		void put(const void *data, size_t num);
		void get(size_t offs, size_t num, void *data);
		void get(const void *indexes, int idx_siz, size_t num, void *data);
	};		
}

#endif //__DS_FILTER_STR_H__
