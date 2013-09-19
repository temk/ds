#ifndef __MX_STRING_ACCESSOR_H__
#define __MX_STRING_ACCESSOR_H__

#include <ds/lookup.h>
#include <ds/utils.h>
using namespace ds;


#include<map>
using namespace std;

#include <mex.h>
#include <matrix.h>
extern "C" mxArray* mxCreateReference(mxArray*);

template<typename T>
class mx_string_accessor : public string_accessor {
private:
  typedef map<const T *, mxArray *> map_t;
  bool singletone_;
  map_t local_map_;

  mxArray * allocString(const string_container &s);
public:
  mx_string_accessor(bool singletone = true);
  ~mx_string_accessor();

  void get(size_t k, const void *p, string_container &s);

  void set(size_t k, const string_container &s, void *p );
};

template<typename T>
mx_string_accessor<T>::mx_string_accessor(bool singletone)
  : singletone_(singletone) {
}

template<typename T>
mx_string_accessor<T>::~mx_string_accessor() {
  for (typename map_t::iterator i = local_map_.begin(); i != local_map_.end(); ++ i) {
    mxDestroyArray(i ->second);
  }
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

  const T *key = (const T*)s.str;
  mxArray *val = NULL;

  if (!singletone_) {
    val = allocString(s);
  } else {
    typename map_t::iterator i = local_map_.find(key);
    if (i == local_map_.end()) {
      mxArray * tmp = allocString(s);
      local_map_.insert(make_pair(key, tmp));
      val = mxCreateReference(tmp);
    } else {
      val = mxCreateReference(i ->second);
    }
  }
  mxSetCell(static_cast<mxArray *>(p), k, val);
}

template<typename T>mxArray *
mx_string_accessor<T>::allocString(const string_container &s) {
  mwSize dim[2] = {1, s.len};
  mxArray *val = mxCreateCharArray(2, &dim[0]);
  ds::str_copy<T, mxChar>(static_cast<const T *>(s.str), mxGetChars(val), s.len);
  return val;
}

#endif // __MX_STRING_ACCESSOR_H__
