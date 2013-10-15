#ifndef __DS_FILTER_DRV_H__
#define __DS_FILTER_DRV_H__
#include <ds/filter.h>

#include <string>
using namespace std;

namespace ds {
	class driver;
	
	class filter_driver : public filter {
	private:
		string key_;
		driver * driver_;
		
	public:
        filter_driver(const error_handler &ref, type_t type, size_t siz, size_t width, const string &key, driver * drv);
		virtual ~filter_driver();		
		
		virtual void flush();
		
		virtual void put(const void *data, size_t num);
		virtual void get(size_t offs, size_t num, void *data);
        virtual void get(const void *indexes, int idx_siz, size_t num, void *data);
	};
}

#endif // __DS_FILTER_DRV_H__
