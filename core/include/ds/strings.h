#ifndef __DS_STRINGS_H__
#define __DS_STRINGS_H__

namespace ds {
  struct string_container {
    int len;
    int siz;
    const void *str;
  };

  class string_accessor {
  public:
    virtual void get(size_t, const void *, string_container &) = 0;
    virtual void set(size_t, const string_container &, void *) = 0;
  };
}

#endif // __DS_STRINGS_H__
