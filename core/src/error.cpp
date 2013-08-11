#include <ds/error.h>

#include <stdexcept>

using namespace ds;
using namespace std;

error_stream::error_stream(severity s) : severity_(s), print_(NULL) {	
}

error_stream::~error_stream() {	
}

error_stream &
error_stream::operator<<(ostream & (*)(ostream &)) const {			
	const char *msg = str_.str().c_str();
	
	if (severity_ == CRITICAL) {
		throw runtime_error(msg);
	} else if (print_) {
		print_(msg);
	}
	
	const_cast<ostringstream &>(str_).str("");
	return const_cast<error_stream &>(*this);
}

void
error_stream::set(print_t p) {
	print_ = p;
}

error_stream::print_t 
error_stream::get() const {
	return print_;
}

error_handler::error_handler(const error_handler &ref) : err(CRITICAL), warn(WARN), info(INFO) {
	err.set(ref.err.get());
	warn.set(ref.warn.get());
	info.set(ref.info.get());
}

error_handler::error_handler() : err(CRITICAL), warn(WARN), info(INFO) {	
}

error_handler::~error_handler() {	
}
