#include <ds/filter_str.h>
#include <ds/lookup.h>
#include <ds/driver.h>
#include <ds/utils.h>

#include <string>

using namespace ds;
using namespace std;

#define KEY(t1, t2) ((t1 << 8) | t2)

lookup::~lookup() {	
}

filter_str::filter_str(type_t t_in, type_t t_out, size_t cap, const string &name, driver *drv) 
	: filter(t_in, size_of(t_in), t_out, size_of(t_out)), cap_(cap), name_(name), driver_(drv) {
		
	switch(KEY(t_in, t_out)) {
	case KEY(DS_T_STRING8, DS_T_UINT8):
		lookup_ = new lookup_impl<string, uint8_t>();
		break;
		
	case KEY(DS_T_STRING8, DS_T_UINT16):
		lookup_ = new lookup_impl<string, uint16_t>();
		break;
		
	case KEY(DS_T_STRING8, DS_T_UINT32):
		lookup_ = new lookup_impl<string, uint32_t>();
		break;
		
	case KEY(DS_T_STRING8, DS_T_UINT64):
		lookup_ = new lookup_impl<string, uint64_t>();
		break;
  // **********************		
	case KEY(DS_T_STRING16, DS_T_UINT8):
		lookup_ = new lookup_impl<basic_string<unsigned short>, uint8_t>();
		break;
		
	case KEY(DS_T_STRING16, DS_T_UINT16):
		lookup_ = new lookup_impl<basic_string<unsigned short>, uint16_t>();
		break;
		
	case KEY(DS_T_STRING16, DS_T_UINT32):
		lookup_ = new lookup_impl<basic_string<unsigned short>, uint32_t>();
		break;
		
	case KEY(DS_T_STRING16, DS_T_UINT64):
		lookup_ = new lookup_impl<basic_string<unsigned short>, uint64_t>();
		break;

	// ************************
	case KEY(DS_T_STRING32, DS_T_UINT8):
		lookup_ = new lookup_impl<wstring, uint8_t>();
		break;
		
	case KEY(DS_T_STRING32, DS_T_UINT16):
		lookup_ = new lookup_impl<wstring, uint16_t>();
		break;
		
	case KEY(DS_T_STRING32, DS_T_UINT32):
		lookup_ = new lookup_impl<wstring, uint32_t>();
		break;
		
	case KEY(DS_T_STRING32, DS_T_UINT64):
		lookup_ = new lookup_impl<wstring, uint64_t>();
		break;

	default:
		throw runtime_error("Unexpected element size");
	}	
	
	buff_ = new char[size_out() * cap_];
	
	size_t len = driver_ ->length(name_);
	if (len > 0) {
		char * dict = new char [len];
		driver_ ->read(name_, 0, dict, len);
		lookup_ ->load(dict, len);
		delete [] dict;
	}
}

filter_str::~filter_str() {
	delete [] buff_;
	delete lookup_;
}

void
filter_str::flush() {
	size_t sz = lookup_ ->size();
	char *buff = new char[sz];
	lookup_ ->dump(buff, sz);
	driver_ ->write(name_, buff, sz, false);
	delete [] buff;
	
	filter::flush();
}

void 
filter_str::put(const void *data, size_t num) {
	const char * src = static_cast<const char *>(data);
	
	while(num > 0) {
		size_t chunk = min(num, cap_);
		lookup_ ->lookup_index(src, buff_, chunk);
		next() ->put(buff_, chunk);
		src += chunk;
		num -= chunk;
	}	
}

void 
filter_str::get(size_t offs, size_t num, void *data) {
	char * dst = static_cast<char *>(data);
	
	while(num > 0) {
		size_t chunk = min(num, cap_);
		next() ->get(offs, chunk, buff_);
		lookup_ ->lookup_value(buff_, dst, chunk);
		dst += chunk;
		num -= chunk;
		offs += chunk * size_in();
	}		
}

void 
filter_str::get(const void *indexes, int idx_siz, size_t num, void *data) {
	char * dst = static_cast<char *>(data);
	const char *idx = static_cast<const char *>(indexes);
	
	while(num > 0) {
		size_t chunk = min(num, cap_);
		next() ->get(idx, idx_siz, chunk, buff_);
		lookup_ ->lookup_value(buff_, dst, chunk);
		dst += chunk;
		num -= chunk;
		idx += chunk * idx_siz;
	}		
}


