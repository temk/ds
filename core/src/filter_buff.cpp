#include <ds/filter_buff.h>
#include <string.h>

using namespace ds;

filter_buff::filter_buff(type_t type, size_t siz, size_t cap) 
 : filter(type, siz, type, siz), cap_(cap), len_(0L) {	
	buff_ = new char [cap * size_in()];
}

filter_buff::~filter_buff() {
	delete [] buff_;
}

void 
filter_buff::flush() {
	next() ->put(buff_, len_);
	len_ = 0L;
}

void 
filter_buff::put(const void *data, size_t num) {
	if ((len_ + num) > cap_) {
		flush();
	}
	
	if (num > cap_) {
		next() ->put(data, num);
	} else {
		memcpy(buff_ + size_in() * len_, data,size_in() * num);
		len_ += num;
	}
}
