#ifndef __DS_MX_STRINGS_H__
#define __DS_MX_STRINGS_H__

#include <ds/utils.h>
#include <ds/strings.h>
using namespace ds;

#include<map>
using namespace std;

#include <mex.h>
extern "C" mxArray* mxCreateReference(mxArray*);

namespace ds {
  template<typename T>
  class mx_local_map {
  private:
    typedef map<const T *, mxArray *> map_t;
    map_t map_;

  public:
    ~mx_local_map();

    mxArray * alloc(const T *s, size_t len);
    mxArray *get(const T *s, size_t len);
  };

  template<typename T>
  class mx_string_accessor : public string_accessor {
  private:
    bool singletone_;
    mx_local_map<T> local_map_;

  public:
    mx_string_accessor(bool singletone = true);

    void get(size_t k, const void *p, string_container &s);

    void set(size_t k, const string_container &s, void *p );
  };
}

template<typename T>
mx_local_map<T>::~mx_local_map() {
  for (typename map_t::iterator i = map_.begin(); i != map_.end(); ++ i) {
    mxDestroyArray(i ->second);
  }
}

template<typename T>mxArray *
mx_local_map<T>::alloc(const T *s, size_t len) {
  mwSize dim[2] = {1, len};
  mxArray *val = mxCreateCharArray(2, &dim[0]);
  ds::str_copy<T, mxChar>(static_cast<const T *>(s), mxGetChars(val), dim[1]);
  return val;
}

template<typename T>mxArray *
mx_local_map<T>::get(const T *key, size_t len) {
  mxArray *val = NULL;

  typename map_t::iterator i = map_.find(key);
  if (i == map_.end()) {
    val = alloc(key, len);
    map_.insert(make_pair(key, val));
  } else {
    val = i ->second;
  }

  return mxCreateReference(val);
}

template<typename T>
mx_string_accessor<T>::mx_string_accessor(bool singletone)
  : singletone_(singletone) {
}

template<typename T>void
mx_string_accessor<T>::get(size_t k, const void *p, string_container &s) {
  const mxArray *arr = mxGetCell(static_cast<const mxArray *>(p), k);
  s.siz = sizeof(mxChar);
  s.len = mxGetNumberOfElements(arr);
  s.str = mxGetData(arr);
}

template<typename T>void
mx_string_accessor<T>::set(size_t k, const string_container &s, void *p ) {
  mxArray *value = NULL;
  if (singletone_) {
    value = local_map_.alloc((const T *)s.str, s.len);
  } else {
    value = local_map_.get((const T *)s.str, s.len);
  }
  mxSetCell(static_cast<mxArray *>(p), k, value);
}

#endif // __DS_MX_STRINGS_H__
