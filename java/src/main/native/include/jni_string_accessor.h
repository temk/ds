#ifndef __JNI_STRING_ACCESSOR_H__
#define __JNI_STRING_ACCESSOR_H__
#include <jni.h>

#include <ds/lookup.h>
using namespace ds;

template<typename T>
class jni_string_accessor : public string_accessor {
private:
  typedef map<const T*, jobject> map_t;

  JNIEnv *env_;
  bool singletone_;
  map_t local_map_;

  jstring allocString(const string_container &s);
public:
  jni_string_accessor(JNIEnv *env, bool singleton = true);
  ~jni_string_accessor();

  void get(size_t k, const void *data, string_container &s);
  void set(size_t k, const string_container &s, void *array );
};

template<typename T>
jni_string_accessor<T>::jni_string_accessor(JNIEnv *env, bool singleton)
  : env_(env), singletone_(singleton) {
}

template<typename T>
jni_string_accessor<T>::~jni_string_accessor() {
  for (typename map_t::iterator i = local_map_.begin(); i != local_map_.end(); ++ i) {
    env_ ->DeleteGlobalRef(i ->second);
  }
}


template<typename T>void
jni_string_accessor<T>::get(size_t k, const void *data, string_container &s) {
  jobjectArray array = (jobjectArray)data;

  jstring str = (jstring )env_ ->GetObjectArrayElement(array, k);
  s.siz = sizeof(jchar);
  s.len = env_ ->GetStringLength(str);
  s.str = env_ ->GetStringCritical(str, 0);

  env_ ->ReleaseStringCritical(str, (jchar *)s.str);
  env_ ->DeleteLocalRef(str);
}

template<typename T>void
jni_string_accessor<T>::set(size_t k, const string_container &s, void *array ) {
  const T *key = (const T*)s.str;
  if (!singletone_) {
    jstring val = allocString(s);
    env_ ->SetObjectArrayElement((jobjectArray)array, k, val);
    env_ ->DeleteLocalRef(val);
  } else {
    typename map_t::iterator i = local_map_.find(key);
    if (i == local_map_.end()) {
      jstring val = allocString(s);
      local_map_.insert(make_pair(key, env_ ->NewGlobalRef(val)));

      env_ ->SetObjectArrayElement((jobjectArray)array, k, val);
      env_ ->DeleteLocalRef(val);
    } else {
      env_ ->SetObjectArrayElement((jobjectArray)array, k, env_ ->NewGlobalRef(i ->second));
    }
  }
}

template<typename T>jstring
jni_string_accessor<T>::allocString(const string_container &s) {
  jstring str = NULL;
  if (s.siz == sizeof(jchar)) {
    str = env_ ->NewString((jchar *)s.str, s.len);
  } else {
    jchar *tmp = new jchar[s.len];
    str_copy<T, jchar>((const T*)s.str, (jchar *)tmp, s.len);
    str = env_ ->NewString(tmp, s.len);
    delete [] tmp;
  }

  return str;
}

#endif // __JNI_STRING_ACCESSOR_H__

