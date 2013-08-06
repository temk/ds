#include <ds/filter.h>

#include <stdexcept>

using namespace ds;

filter::filter(type_t t_in, size_t s_in, type_t t_out, size_t s_out) 
	: s_in_(s_in), s_out_(s_out), t_in_(t_in), t_out_(t_out), next_(0L) {
}

filter::~filter() {	
	if (next_) {
		delete next_;
	}
}
		
void 
filter::flush() {
	next_ ->flush();
}

void 
filter::put(const void *data, size_t num) {
	next_ ->put(data, num);
}

void 
filter::get(size_t offs, size_t num, void *data) {
	next_ ->get(offs, num, data);
}

void 
filter::get(const void *indexes, int idx_siz, size_t num, void *data) {
	next_ -> get(indexes, idx_siz, num, data);
}

void 
filter::next(filter *f) {
	if (f != NULL && (s_out_ != f ->s_in_ || t_out_ != f ->t_in_ ))
		throw runtime_error("filter::next: incompatible filters");
	next_ = f;
}


