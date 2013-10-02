#ifndef __CSV2DS_H__
#define __CSV2DS_H__

#include <ds/types.h>
#include <ds/error.h>
#include <ds/column.h>
using namespace ds;

#include <string>
#include <vector>
using namespace std;

#define DEFAULT_LINE_LENGTH 1024

namespace csv2ds {
  struct user_meta {
    bool global;
    int col_num;
    string col_name;
    string key;
    string val;
  };

  struct options {
    int col_limit;
    int guess_rows;
    bool guess_tight;
    bool head;
    bool force;
    char delim;
    char quote;
    string input;
    string output;
    type_t str_type;
    vector<type_t> types;
    vector<string> names;
    vector< pair<string, type_t> > name_with_type; // option -t<name>
    vector<user_meta> meta;
  };

  void usage(ostream &out, const string &prog);
  int parse_params(int argc, char **argv, options &opt);

  typedef bool (* appender_t)(column &, const char *s, const char *e, const options &opt);

  appender_t create_appender(type_t type);
}

#endif //__CSV2DS_H__
