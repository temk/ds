#include <ds/filter_compress.h>
#include <stdio.h>

using namespace ds;

template<typename T>
class filter_compress_impl : public filter
{
private:
    T elem_in_;
    T elem_out_;
    size_t count_in_;
    size_t count_out_;

public:
    filter_compress_impl(const error_handler &ref, type_t t_in, size_t s_in);

    void flush();
    void put(const void *data, size_t num);
    void get(size_t offs, size_t num, void *data);
};

template<typename T>
filter_compress_impl<T>::filter_compress_impl(const error_handler &ref, type_t t_in, size_t s_in)
 : filter(ref, t_in, s_in, DS_T_UINT8, 1, 1), count_in_(0), count_out_(0)
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
filter_compress_impl<T>::get(size_t offs, size_t num, void *data)
{
    if (offs != 0)
    {
        err << "offs != 0" << endl;
    }

    T *d = reinterpret_cast<T *>(data);
    size_t o = 0;
    for (; num; -- num, ++ d)
    {
        if (count_in_ == 0)
        {
            next() ->get(o, sizeof(T), &elem_in_);
            o += sizeof(T);
            next() ->get(o, sizeof(count_in_), &count_in_);
            o += sizeof(count_in_);
            //fprintf(stderr, "get element %d: %d\n", int(elem_in_), int(count_in_));
        }

        *d = elem_in_;
        -- count_in_;
    }
}

filter_compress::filter_compress(const error_handler &ref, type_t type, size_t siz, size_t width)
  : filter(ref, type, siz, DS_T_UINT8, 1, 1) {

    switch(type)
    {
    case DS_T_BOOL:
    case DS_T_INT8:
    case DS_T_UINT8:
        sf_ = new filter_compress_impl<uint8_t>(ref, type, siz);
        break;

    case DS_T_INT16:
    case DS_T_UINT16:
        sf_ = new filter_compress_impl<uint16_t>(ref, type, siz);
        break;

    case DS_T_INT32:
    case DS_T_UINT32:
    case DS_T_FLOAT32:
        sf_ = new filter_compress_impl<uint32_t>(ref, type, siz);
        break;

    case DS_T_INT64:
    case DS_T_UINT64:
    case DS_T_FLOAT64:
        sf_ = new filter_compress_impl<uint64_t>(ref, type, siz);
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
    sf_ ->put(data, num);
}

void
filter_compress::get(size_t offs, size_t num, void *data)
{
    if (offs > 0) {
        err << "not implemented" << endl;
    }

    sf_ ->get(0, num, data);
}

void
filter_compress::get(const void *indexes, int idx_siz, size_t num, void *data)
{
        err << "not implemented" << endl;
}
