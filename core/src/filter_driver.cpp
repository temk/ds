#include <ds/filter_driver.h>
#include <ds/driver.h>

using namespace ds;
using namespace std;

filter_driver::filter_driver(const error_handler &ref, type_t type, size_t siz, const string &key, driver * drv) 
	: filter(ref, type, siz, DS_T_INVALID, 0), key_(key), driver_(drv) {	
}

filter_driver::~filter_driver() {	
}
		
void 
filter_driver::flush() {
	// do nothing
}
		
void 
filter_driver::put(const void *data, size_t num) {
	driver_ ->write(key_, data, size_in() * num);
}

void 
filter_driver::get(size_t offs, size_t num, void *data) {
	driver_ ->read(key_, size_in() *offs, data, size_in() * num);
}

void 
filter_driver::get(const void *indexes, int idx_siz, size_t num, void *data) {
	driver_ ->read(key_, indexes, idx_siz, num, data, size_in());
}
