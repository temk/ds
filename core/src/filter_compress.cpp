#include <ds/filter_compress.h>

using namespace ds;

filter_compress::filter_compress(const error_handler &ref, type_t type, size_t siz, size_t width, size_t cap)
 : filter(ref, type, siz, type, siz, width), cap_(cap), len_(0L) {
    buff_ = new char [cap * size_in() * width];
}

filter_compress::~filter_compress() {
	delete [] buff_;
}

void
filter_compress::flush() {
	if (len_ > 0) {
		next() ->put(buff_, len_);
		len_ = 0L;
	}
}
