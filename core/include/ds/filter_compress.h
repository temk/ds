#ifndef __DS_FILTER_COMPRESS_H__
#define __DS_FILTER_COMPRESS_H__
#include <ds/filter.h>

namespace ds {
	class filter_compress : public filter {
	private:
		size_t cap_;
		size_t len_;
		char *buff_;

	public:
        filter_compress(const error_handler &ref, type_t type, size_t siz, size_t width, size_t cap);
		~filter_compress();

		void flush();
		void put(const void *data, size_t num);

		inline size_t capacity() const;
		inline size_t length() const;
	};

	// =============================================================
	inline size_t filter_compress::capacity() const {
		return cap_;
	}

	inline size_t filter_compress::length() const {
		return len_;
	}
}

#endif // __DS_FILTER_COMPRESS_H__
