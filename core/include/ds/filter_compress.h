#ifndef __DS_FILTER_COMPRESS_H__
#define __DS_FILTER_COMPRESS_H__
#include <ds/filter.h>

#define DEFAULT_COMPRED_READ_BUF_SIZ (1 << 20)
namespace ds {
	class filter_compress : public filter {
	private:
        filter *sf_;

	public:
        filter_compress(const error_handler &ref, type_t type, size_t siz, size_t width, size_t buf_siz);
		~filter_compress();

		void flush();
		void put(const void *data, size_t num);

		void get(size_t offs, size_t num, void *data);
        void get(const void *indexes, int idx_siz, size_t num, void *data);
        void next(filter *);
	};
}

#endif // __DS_FILTER_COMPRESS_H__
