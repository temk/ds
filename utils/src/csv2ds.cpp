#include <csv2ds.h>

#include <ds/utils.h>
#include <ds/storage.h>

using namespace ds;
using namespace csv2ds;

#include <string.h>
#include <stdlib.h>

#include <string>
#include <limits>
#include <fstream>
#include <iostream>
#include <stdexcept>
using namespace std;

static int convert(istream &in, storage &stor, options &opt);
static void count_columns(const string &, options &opt);
static bool split(const string &line, vector<const char *> &s, vector<const char *> &e, options &opt);
static void set_names(vector<const char *> &s, vector<const char *> &e, options &opt);
static void remove_quote(const char *&s, const char *&e, char quote);
static void fill_types_by_name(options &opt);
static void fill_types(istream &in, string line, vector<string> &cache, options &opt);
static type_t guess_type(const char *s, const char *e, options &opt);
static void create_columns(storage &ds, vector<appender_t> &appenders, options &opt);

#define INPUT_BUF_SIZ (1 << 20) // 1M
int
main(int argc, char **argv) {
  options opt;
  int ret = parse_params(argc, argv, opt);
  if (ret < 0) {
    return ret;
  } else if (ret > 0) {
    return 0;
  }
  try {
    int flags = DS_O_WRITE|DS_O_CREATE;
    if (opt.force) {
      flags |= DS_O_TRUNC;
    }

    storage stor(opt.output, flags);

    if (opt.input == "-") {
      ret = convert(cin, stor, opt);
    } else {
      ifstream in(opt.input.c_str());

      char buffer[INPUT_BUF_SIZ];
      in.rdbuf() ->pubsetbuf(buffer, INPUT_BUF_SIZ);

      ret = convert(in, stor, opt);
    }
  } catch(exception &ex) {
    cerr << "Runtime Error: " << ex.what() << endl;
    ret = -1;
  }

  return ret;
}

int
convert(istream &in, storage &stor, options &opt) {
  string line;
  vector<string> cache;


  getline(in, line);
  count_columns(line, opt);

  opt.names.resize(opt.col_limit);

  vector<const char *> s_token;
  vector<const char *> e_token;

  s_token.resize(opt.col_limit, NULL);
  e_token.resize(opt.col_limit, NULL);

  if (opt.head) {
    split(line, s_token, e_token, opt);
    set_names(s_token, e_token, opt);

    getline(in, line);
  }

  fill_types_by_name(opt);
  cache.push_back(line);
  fill_types(in, line, cache, opt);

  vector<appender_t> appenders;
  create_columns(stor, appenders, opt);
  stor.flush();

  int rows = 0;
  for (int k = 0; k < cache.size(); ++ k) {
    if (!split(cache[k], s_token, e_token, opt)) {
      cerr << "Error at row " << rows << endl;
      return -1;
    }
    for (int col = 0; col < opt.col_limit; ++ col) {
      if (!appenders[col](stor.column_at(col), s_token[col], e_token[col], opt)) {
        cerr << "Fail to parse value at row "
             << (rows + 1) << ", column " << (col + 1)
             << ":=>" << string(s_token[col], e_token[col] - s_token[col]) << endl;
        return -1;
      }
    }

    ++ rows;
  }

  while(getline(in, line) && line.length() > 0) {
    if (!split(line, s_token, e_token, opt)) {
      cerr << "Error at row " << rows << endl;
      return -1;
    }
    for (int col = 0; col < opt.col_limit; ++ col) {
      if (!appenders[col](stor.column_at(col), s_token[col], e_token[col], opt)) {
        cerr << "Fail to parse value at row "
             << (rows + 1) << ", column " << (col + 1)
             << ":=>" << string(s_token[col], e_token[col] - s_token[col]) << endl;
        return -1;
      }
    }

    ++ rows;
 }

  stor.flush();
//  cout << "read " << rows << " rows." << endl;
  return 0;
}

void
count_columns(const string &line, options &opt) {
  int num = 1;
  bool in_quote = false;
  for (const char *p = line.c_str(); *p; ++ p) {
    if (*p == opt.quote && p > line.c_str() && *(p - 1) != '\\') {
      in_quote = !in_quote;
    }

    if (!in_quote && *p == opt.delim) {
      ++ num;
    }
  }

  if (opt.col_limit == 0) {
    opt.col_limit = num;
  } else if (opt.col_limit > num) {
    opt.col_limit = num;
  }
}

bool
split(const string &line, vector<const char *> &s, vector<const char *> &e, options &opt) {
  int s_idx = 0, e_idx = 0;
  const char *p = line.c_str();

  bool in_quote = false;

  s[s_idx ++] = p;
  for (; *p; ++ p) {

    if (*p == opt.quote && p > line.c_str() && *(p - 1) != '\\') {
      in_quote = !in_quote;
    }

    if (!in_quote && *p == opt.delim) {

      e[e_idx ++] = p - 1;
      if (s_idx < opt.col_limit) {
        s[s_idx ++] = p + 1;
      } else {
        break;
      }
    }
  }

  if (e_idx < s_idx) {
    e[e_idx] = p - 1;
  }

  if (s_idx < opt.col_limit) {
    return false;
  }

  return true;
}

void
set_names(vector<const char *> &s, vector<const char *> &e, options &opt) {
  for (int k = 0; k < opt.col_limit; ++ k) {
    if (opt.names[k].empty()) {
      remove_quote(s[k], e[k], opt.quote);
      string name(s[k], e[k] - s[k] + 1);
      opt.names[k] = name;
    }
  }
}

void
remove_quote(const char *&s, const char *&e, char quote) {
  for (;*s == quote && *e == quote; ++ s, -- e);
}

void
fill_types_by_name(options &opt) {
  for (int j = 0; j < opt.name_with_type.size(); ++ j) {
    for (int k = 0; k < opt.names.size(); ++ k) {
      if (opt.names[k] == opt.name_with_type[j].first)  {
        opt.types[k] = opt.name_with_type[j].second;
        continue;
      }
    }
  }
}

void
fill_types(istream &in, string line, vector<string> &cache, options &opt) {
  vector<const char *> s_token;
  vector<const char *> e_token;

  s_token.resize(opt.col_limit);
  e_token.resize(opt.col_limit);

  opt.types.resize(opt.col_limit, DS_T_INVALID);

  vector<type_t> found_types(opt.types.begin(), opt.types.end());

  for (int r = 0; r < opt.guess_rows; ++ r) {
    split(line, s_token, e_token, opt);

    for (int k = 0; k < opt.col_limit; ++ k) {
      if (opt.types[k] == DS_T_INVALID) {
        type_t t = guess_type(s_token[k], e_token[k], opt);
        if (found_types[k] < t) {
          found_types[k] = t;
        }
      }
    }

    getline(in, line);
    cache.push_back(line);
  }

  for (int k = 0; k < opt.col_limit; ++ k) {
    if (opt.types[k] == DS_T_INVALID) {
      if (opt.guess_tight || is_str(found_types[k])) {
        opt.types[k] = found_types[k];
      } else {
        opt.types[k] = DS_T_FLOAT64;
      }
    }
  }
}

type_t
guess_type(const char *s, const char *e, options &opt) {
  if (*s == opt.quote) {
    return opt.str_type;
  }

  if ((e - s) == 3 && strncasecmp(s, "nan", 3) == 0) {
    return DS_T_FLOAT32;
  }

  if ((e - s) == 1 && (*s == 1 || *s == 0)) {
    return DS_T_BOOL;
  }

  char *p = NULL;
  long l = strtol(s, &p, 10);
  if (p == e + 1) {
    if (l <= numeric_limits<int8_t>::max() && l >= numeric_limits<int8_t>::min())
        return DS_T_INT8;
    else if (l <= numeric_limits<int16_t>::max() && l >= numeric_limits<int16_t>::min())
      return DS_T_INT16;
    else if (l <= numeric_limits<int32_t>::max() && l >= numeric_limits<int32_t>::min())
      return DS_T_INT32;
    else
      return DS_T_INT64;
  }

  double d = strtod(s, &p);
  if (p == e + 1) {
    return DS_T_FLOAT64;
  }

  return opt.str_type;
}

void
create_columns(storage &ds, vector<appender_t> &appenders, options &opt) {
  for (size_t k = 0; k < opt.col_limit; ++ k) {
    if (opt.types[k] == DS_T_INVALID) {
      throw runtime_error("column has type INVALID");
    }

    if (is_str(opt.types[k])) {
      ds.add(opt.types[k], DS_T_UINT32, opt.names[k]);
    } else {
      ds.add(opt.types[k], opt.names[k]);
    }

    appenders.push_back(create_appender(opt.types[k]));
  }

  ds.flush();
}


