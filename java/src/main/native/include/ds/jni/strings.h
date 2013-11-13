#ifndef __DS_JNI_STRINGS_H__
#define __DS_JNI_STRINGS_H__
#include <jni.h>

#include <ds/utils.h>
#include <ds/strings.h>

#include <map>
#include <algorithm>
#include <stdexcept>

namespace ds {
  template<typename T>
  class jni_local_map {
  private:
    typedef std::map<const T*, jstring> map_t;

    map_t map_;
    JNIEnv *env_;


  public:
    jni_local_map(JNIEnv *env);
    ~jni_local_map();

    jstring alloc(const T *s, size_t len);
    jstring get(const T *s, size_t len);
  };

  template<typename T>
  class jni_string_accessor : public string_accessor {
  private:

    JNIEnv *env_;
    bool singletone_;
    jni_local_map<T> local_map_;

  public:
    jni_string_accessor(JNIEnv *env, bool singleton = true);

    void get(size_t k, const void *data, string_container &s);
    void set(size_t k, const string_container &s, void *array );
  };
}

using namespace ds;

template<typename T>
jni_local_map<T>::jni_local_map(JNIEnv *env) : env_(env) {
}

template<typename T>
jni_local_map<T>::~jni_local_map() {
  for (typename map_t::iterator i = map_.begin(); i != map_.end(); ++ i) {
    env_ ->DeleteGlobalRef(i ->second);
  }
}

template<typename T>jstring
jni_local_map<T>::alloc(const T *s, size_t len) {
  jstring str = NULL;
  if (sizeof(T) == sizeof(jchar)) {
    str = env_ ->NewString((jchar *)s, len);
  } else {
    jchar *tmp = new jchar[len];
    str_copy<T, jchar>((const T*)s, (jchar *)tmp, len);
    str = env_ ->NewString(tmp, len);
    delete [] tmp;
  }

  return str;
}

template<typename T>jstring
jni_local_map<T>::get(const T *s, size_t len) {
  typename map_t::iterator i = map_.find(s);
  if (i == map_.end()) {
    jstring val = alloc(s, len);
    map_.insert(std::make_pair(s, (jstring )env_ ->NewGlobalRef(val)));
    i = map_.find(s);
    env_ ->DeleteLocalRef(val);
  }
  return i ->second;
}


template<typename T>
jni_string_accessor<T>::jni_string_accessor(JNIEnv *env, bool singleton)
  : env_(env), singletone_(singleton), local_map_(env) {
}


template<typename T>void
jni_string_accessor<T>::get(size_t k, const void *data, string_container &s) {

  jobjectArray array = (jobjectArray)data;
  jstring str = (jstring )env_ ->GetObjectArrayElement(array, k);

  if (str == NULL) {
      throw std::runtime_error("jni_string_accessor<T>::get: Null Pointer");
  }

  s.siz = sizeof(jchar);
  s.len = env_ ->GetStringLength(str);
  s.str = env_ ->GetStringCritical(str, 0);

  env_ ->ReleaseStringCritical(str, (jchar *)s.str);
  env_ ->DeleteLocalRef(str);
}

template<typename T>void
jni_string_accessor<T>::set(size_t k, const string_container &s, void *array ) {
  if (!singletone_) {
    jstring val = local_map_.alloc((const T *)s.str, s.len);
    env_ ->SetObjectArrayElement((jobjectArray)array, k, val);
    env_ ->DeleteLocalRef(val);
  } else {
    env_ ->SetObjectArrayElement((jobjectArray)array, k, local_map_.get((const T *)s.str, s.len));
  }
}

#endif // __DS_JNI_STRINGS_H__

