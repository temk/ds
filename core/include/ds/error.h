#ifndef __DS_ERROR_H__
#define __DS_ERROR_H__

#include <sstream>

using namespace std;

namespace ds {
	enum severity {
		INFO,
		WARN,
		CRITICAL
	};

	class error_stream {
	private:
		typedef void (* print_t) (const char *);

		print_t print_;
		ostringstream str_;
		severity severity_;

	public:
		error_stream(severity);
		~error_stream();

		template<typename T> inline error_stream &operator<<(const T &val) const;
		error_stream &operator<<(ostream & (*)(ostream &)) const;

		 void set(print_t p);
		 print_t get() const;
	};

	class error_handler {
	public:
		error_stream err;
		error_stream warn;
		error_stream info;

		error_handler();
		error_handler(const error_handler &ref);
		virtual ~error_handler();
	};

	template<typename T>inline error_stream &
	error_stream::operator<<(const T &val) const {
		const_cast<ostringstream &>(str_) << val;
		return const_cast<error_stream &>(*this);
	}
}

#endif // __DS_ERROR_HANDLER_H__
