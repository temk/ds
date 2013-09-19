#include <ds/filter_str.h>
#include <ds/lookup.h>
#include <ds/driver.h>
#include <ds/utils.h>

#include <string>

using namespace ds;
using namespace std;


filter_str::filter_str(const error_handler &ref, type_t t_in, type_t t_out, size_t cap, const string &key, driver * drv, bool read_dict) 
	: filter(ref, t_in, size_of(t_in), t_out, size_of(t_out)), cap_(cap), driver_(drv) {

	lookup_ = lookup::create(ref, key, t_in, t_out);
	if (read_dict) {
		driver_ ->read_dictionary(*lookup_);
	}
	buff_ = new char[size_out() * cap_];
}

filter_str::~filter_str() {
	delete [] buff_;
}

void
filter_str::flush() {	
	filter::flush();
	driver_ ->write_dictionary(*lookup_);
}

void 
filter_str::put(const void *data, size_t num) {
	size_t adv = 0;
	while(num > 0) {
		size_t chunk = min(num, cap_);
		lookup_ ->lookup_index(data, buff_, adv, chunk);
		next() ->put(buff_, chunk);
		adv += chunk;
		num -= chunk;
	}	
}

void 
filter_str::get(size_t offs, size_t num, void *data) {
	size_t adv = 0;
	while(num > 0) {
		size_t chunk = min(num, cap_);
		next() ->get(offs, chunk, buff_);
		lookup_ ->lookup_value(buff_, data, adv, chunk);
		num -= chunk;
		adv += chunk;
		offs += chunk;
	}		
}

void 
filter_str::get(const void *indexes, int idx_siz, size_t num, void *data) {
	size_t adv = 0;
	const char *idx = static_cast<const char *>(indexes);
	
	while(num > 0) {
		size_t chunk = min(num, cap_);
		next() ->get(idx, idx_siz, chunk, buff_);
		lookup_ ->lookup_value(buff_, data, adv, chunk);
		num -= chunk;
		adv += chunk;
		idx += chunk * idx_siz;
	}		
}

void 
filter_str::set_string_accessor(string_accessor *acc) {
	lookup_ ->set_string_accessor(acc);
}


