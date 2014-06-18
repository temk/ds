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
    JavaVM *jvm_;


  public:
    jni_local_map(JNIEnv *env);
    ~jni_local_map();

    JNIEnv *environment();

    jstring alloc(const T *s, size_t len);
    jstring get(const T *s, size_t len);
  };

  template<typename T>
  class jni_string_accessor : public string_accessor {
  private:

    bool singletone_;
    jni_local_map<T> local_map_;
  
    JNIEnv *environment() { return local_map_.environment(); }

  public:
    jni_string_accessor(JNIEnv *env, bool singleton = true);
    void get(size_t k, const void *data, string_container &s);
    void set(size_t k, const string_container &s, void *array );
  };
}

using namespace ds;

template<typename T>
jni_local_map<T>::jni_local_map(JNIEnv *env) {
  env ->GetJavaVM(&jvm_);
}

template<typename T>
jni_local_map<T>::~jni_local_map() {
  JNIEnv * env = environment();
  for (typename map_t::iterator i = map_.begin(); i != map_.end(); ++ i) {
    env ->DeleteGlobalRef(i ->second);
  }
}


template<typename T>JNIEnv *
jni_local_map<T>::environment() {
   JNIEnv *env = NULL;

  int result = jvm_->GetEnv((void **)&env, JNI_VERSION_1_6);

  if (result == JNI_EDETACHED) {
      if (jvm_->AttachCurrentThread((void **) &env, NULL) != 0) {
          env = NULL;
      }
  } else if (result == JNI_EVERSION) {
      env = NULL;
  }

  return env;
 
}

template<typename T>jstring
jni_local_map<T>::alloc(const T *s, size_t len) {
  JNIEnv * env = environment();
  jstring str = NULL;
  if (sizeof(T) == sizeof(jchar)) {
    str = env ->NewString((jchar *)s, len);
  } else {
    jchar *tmp = new jchar[len];
    str_copy<T, jchar>((const T*)s, (jchar *)tmp, len);
    str = env ->NewString(tmp, len);
    delete [] tmp;
  }

  return str;
}

template<typename T>jstring
jni_local_map<T>::get(const T *s, size_t len) {
  JNIEnv * env = environment();
  typename map_t::iterator i = map_.find(s);
  if (i == map_.end()) {
    jstring val = alloc(s, len);
    map_.insert(std::make_pair(s, (jstring )env ->NewGlobalRef(val)));
    i = map_.find(s);
    env ->DeleteLocalRef(val);
  }
  return i ->second;
}


template<typename T>
jni_string_accessor<T>::jni_string_accessor(JNIEnv *env, bool singleton)
  : singletone_(singleton), local_map_(env) {
}


template<typename T>void
jni_string_accessor<T>::get(size_t k, const void *data, string_container &s) {
  JNIEnv * env = environment();
  jobjectArray array = (jobjectArray)data;
  jstring str = (jstring )env ->GetObjectArrayElement(array, k);

  if (str == NULL) {
      throw std::runtime_error("jni_string_accessor<T>::get: Null Pointer");
  }

  s.siz = sizeof(jchar);
  s.len = env ->GetStringLength(str);
  s.str = env ->GetStringCritical(str, 0);

  env ->ReleaseStringCritical(str, (jchar *)s.str);
  env ->DeleteLocalRef(str);
}

template<typename T>void
jni_string_accessor<T>::set(size_t k, const string_container &s, void *array ) {
  JNIEnv * env = environment();
  if (!singletone_) {
    jstring val = local_map_.alloc((const T *)s.str, s.len);
    env ->SetObjectArrayElement((jobjectArray)array, k, val);
    env ->DeleteLocalRef(val);
  } else {
    env ->SetObjectArrayElement((jobjectArray)array, k, local_map_.get((const T *)s.str, s.len));
  }
}

#endif // __DS_JNI_STRINGS_H__

