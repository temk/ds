#include <csv2ds.h>
#include <ds/utils.h>

using namespace ds;
using namespace csv2ds;

#include <string.h>
#include <stdlib.h>
#include <limits>
using namespace std;


template<typename T> bool
append_str(column &col, const char *s, const char *e, const options &opt) {
  T* val = str_dup<T, char>(s, e - s + 1);
  col.append(&val, 1);
  delete [] val;
  return true;
}

template<typename T> bool
append_int(column &col, const char *s, const char *e, const options &opt) {
  char *p = NULL;
  T val = static_cast<T>(strtol(s, &p, 10));
  if (p != e + 1) {
    return false;
  }
  col.append(&val, 1);
  return true;
}

template<typename T> bool
append_float(column &col, const char *s, const char *e, const options &opt) {
  T val;

  if ((e - s == 3) && strcasecmp(s, "nan") == 0) {
    val = numeric_limits<T>::quiet_NaN();
  } else if ((e - s == 3) && strcasecmp(s, "inf") == 0) {
    val = numeric_limits<T>::infinity();
  } else if ((e - s == 4) && strcasecmp(s, "-inf") == 0) {
    val = - numeric_limits<T>::infinity();
  } else {
    char * p = NULL;
    val = static_cast<T>(strtod(s, &p));
    if (p != e + 1) {
      return false;
    }
  }

  col.append(&val, 1);
  return true;
}

appender_t
csv2ds::create_appender(type_t t) {
  switch(t) {
  case DS_T_BOOL:
    return &append_int<bool>;

  case DS_T_INT8:
    return &append_int<int8_t>;

  case DS_T_INT16:
    return &append_int<int16_t>;

  case DS_T_INT32:
    return &append_int<int32_t>;

  case DS_T_INT64:
    return &append_int<int64_t>;

  case DS_T_UINT8:
    return &append_int<uint8_t>;

  case DS_T_UINT16:
    return &append_int<uint16_t>;

  case DS_T_UINT32:
    return &append_int<uint32_t>;

  case DS_T_UINT64:
    return &append_int<uint64_t>;

  case DS_T_FLOAT32:
    return &append_float<float32_t>;

  case DS_T_FLOAT64:
    return &append_float<float64_t>;

  case DS_T_STRING8:
    return &append_str<int8_t>;

  case DS_T_STRING16:
    return &append_str<int16_t>;

  case DS_T_STRING32:
    return &append_str<int32_t>;

  case DS_T_INVALID:
  default:
    return NULL;
  }
}


//// ======================================================================
//template<typename T>T
//read_float(const string & s) {
//  T val;
//  stringstream str(s);

//  str >> val;


//  return val;
//}


//template <>void
//appender_ex<double>::append(const string &s) {
//  double val = read_float<double>(s);
//  col_.append(&val, 1);
//}

//template <>void
//appender_ex<float>::append(const string &s) {
//  float val = read_float<float>(s);
//  col_.append(&val, 1);
//}

