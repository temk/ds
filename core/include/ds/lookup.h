#ifndef __DS_LOOKUP_H__
#define __DS_LOOKUP_H__
#include <ds/types.h>
#include <ds/error.h>
#include <ds/utils.h>

using namespace std;
namespace ds {
  class string_accessor;

  class lookup : public error_handler {
  public:
    virtual ~lookup();
    virtual void lookup_index(const void *str, void *val, size_t from, size_t num) = 0;
    virtual void lookup_value(const void *val, void *str, size_t from, size_t num) = 0;

    virtual size_t count() const = 0;
    virtual void get(void *str, size_t *len, int &el_siz) const = 0;
    virtual void set(const void *str, size_t total_bytes) = 0;

    virtual void set_string_accessor(string_accessor *) = 0;

    virtual const string &key() const = 0;
    static lookup *create(const error_handler &ref, const string &key, type_t int_type, type_t ext_type);
  };
}


#endif //__DS_LOOKUP_H__
