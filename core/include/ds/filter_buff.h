#ifndef __DS_FILTER_BUFF_H__
#define __DS_FILTER_BUFF_H__
#include <ds/filter.h>

namespace ds {
	class filter_buff : public filter {
	private:
		size_t cap_;
		size_t len_;
		char *buff_;

	public:
        filter_buff(const error_handler &ref, type_t type, size_t siz, size_t width, size_t cap);
		~filter_buff();

		void flush();
		void put(const void *data, size_t num);

		inline size_t capacity() const;
		inline size_t length() const;
	};

	// =============================================================
	inline size_t filter_buff::capacity() const {
		return cap_;
	}

	inline size_t filter_buff::length() const {
		return len_;
	}
}

#endif //__DS_FILTER_BUFF_H__
