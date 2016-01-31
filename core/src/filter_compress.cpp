#include <ds/filter_compress.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

using namespace ds;

template<typename T>
class filter_compress_impl : public filter
{
private:
    T elem_in_;
    T elem_out_;
    size_t count_in_;
    size_t count_out_;
    size_t buf_siz_;
public:
    filter_compress_impl(const error_handler &ref, type_t t_in, size_t s_in, size_t buf_siz);

    void flush();
    void put(const void *data, size_t num);
    void get(size_t offs, size_t num, void *data);
};

template<typename T>
filter_compress_impl<T>::filter_compress_impl(const error_handler &ref, type_t t_in, size_t s_in, size_t buf_siz)
 : filter(ref, t_in, s_in, DS_T_UINT8, 1, 1), count_in_(0), count_out_(0), buf_siz_(buf_siz)
 {}

template<typename T>void
filter_compress_impl<T>::flush()
{
    if (count_out_ > 0)
    {
        //fprintf(stderr, "fut element %d: %d\n", int(elem_out_), int(count_out_));
        next() ->put(&elem_out_, sizeof(T));
        next() ->put(&count_out_, sizeof(size_t));
        count_out_ = 0;
    }

    next() ->flush();
}

template<typename T>void
filter_compress_impl<T>::put(const void *data, size_t num)
{
    const T *d = reinterpret_cast<const T *>(data);
    for (; num; -- num, ++ d)
    {
        if (count_out_ == 0) {
            elem_out_ = *d;
        } else if (elem_out_ != *d) {
            //fprintf(stderr, "put element %d: %d\n", int(elem_out_), int(count_out_));
            next() ->put(&elem_out_, sizeof(T));
            next() ->put(&count_out_, sizeof(size_t));
            count_out_ = 0;
            elem_out_ = *d;
        }
        ++ count_out_;
    }
}

template<typename T>void
filter_compress_impl<T>::get(size_t offs, size_t num, void *data) {
    T *d = reinterpret_cast<T *>(data);

    size_t adv = 0;
    size_t has = next() ->available();

    size_t len = 0;
    size_t ptr = 0;
    size_t tot = 0;
    char * buf = new char[buf_siz_];



    for (size_t k = 0; k < num + offs; ++ k) {

        if (count_in_ == 0) {
            if (len - ptr < sizeof(elem_in_) + sizeof(count_in_)) {
                if (len > ptr) {
                    memmove(buf, buf + ptr, len - ptr);
                    ptr = len - ptr;
                } else {
                    ptr = 0;
                }

                size_t rd = min(has, buf_siz_ - ptr);
                next() ->get(adv, rd, buf + ptr);
                len = rd + ptr;
                adv += rd;
                has -= rd;
            }

            memcpy(&elem_in_, &buf[ptr], sizeof(elem_in_));
            ptr += sizeof(elem_in_);

            memcpy(&count_in_, &buf[ptr], sizeof(count_in_));
            ptr += sizeof(count_in_);
        }

        -- count_in_;

        if (k >= offs) {
            *d = elem_in_;
            ++ d;
        }
    }
}

filter_compress::filter_compress(const error_handler &ref, type_t type, size_t siz, size_t width, size_t buf_siz)
  : filter(ref, type, siz, DS_T_UINT8, 1, width) {

    switch(type)
    {
    case DS_T_BOOL:
    case DS_T_INT8:
    case DS_T_UINT8:
        sf_ = new filter_compress_impl<uint8_t>(ref, type, siz, buf_siz);
        break;

    case DS_T_INT16:
    case DS_T_UINT16:
        sf_ = new filter_compress_impl<uint16_t>(ref, type, siz, buf_siz);
        break;

    case DS_T_INT32:
    case DS_T_UINT32:
    case DS_T_FLOAT32:
        sf_ = new filter_compress_impl<uint32_t>(ref, type, siz, buf_siz);
        break;

    case DS_T_INT64:
    case DS_T_UINT64:
    case DS_T_FLOAT64:
        sf_ = new filter_compress_impl<uint64_t>(ref, type, siz, buf_siz);
        break;

    default:
        err << "Unexpected type" << endl;
    }

}

void
filter_compress::next(filter *f) {
    sf_ ->next(f);
    this ->filter::next(f);
}

filter_compress::~filter_compress() {
	delete sf_;
}

void
filter_compress::flush() {
    sf_ ->flush();
}

void
filter_compress::put(const void *data, size_t num)
{
    sf_ ->put(data, num * width());
}

void
filter_compress::get(size_t offs, size_t num, void *data)
{
    sf_ ->get(offs * width(), num * width(), data);
}

void
filter_compress::get(const void *indexes, int idx_siz, size_t num, void *data)
{
        err << "not implemented" << endl;
}
