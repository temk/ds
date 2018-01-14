#include <ds/filter_buff.h>
#include <string.h>

using namespace ds;

filter_buff::filter_buff(const error_handler &ref, type_t type, size_t siz, size_t width, size_t cap)
 : filter(ref, type, siz, type, siz, width), cap_(cap), len_(0L) {
    buff_ = new char [cap * size_in() * width];
}

filter_buff::~filter_buff() {
	delete [] buff_;
}

void
filter_buff::flush() {
	if (len_ > 0) {
		next() ->put(buff_, len_);
		len_ = 0L;
	}
}

void
filter_buff::put(const void *data, size_t num) {
	if ((len_ + num) > cap_) {
		flush();
	}

	if (num > cap_) {
		next() ->put(data, num);
	} else {
      memcpy(buff_ + size_in() * len_ * width(), data, size_in() * num * width());
		len_ += num;
	}
}
