#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <sstream>
#include <cstdlib>
#include <limits>

#include <string.h>

#include <ds/utils.h>
#include <ds/column.h>
#include <ds/storage.h>

using namespace ds;
using namespace std;

#define TAB "\t"

struct options {
  bool head;
  bool force;
  bool guess;
  string delim;
  string quote;
  string input;
  string output;
  type_t str_type;
  vector<type_t> types;
  vector<string> names;
};

class appender {
protected:
  column &col_;

public:
  appender(column &col);
  virtual ~appender();

  virtual void append(const string &) = 0;

  inline column &col() { return col_; }
};

template <typename T>
class appender_ex : public appender {
public:
  appender_ex(column &col) : appender(col) {
  }

  void append(const string &s) {
    T val;
    stringstream str(s);

    str >> val;

    col_.append(&val, 1);
  }
};

template <>void 
appender_ex<str8_t>::append(const string &s) {
  const char * str = s.c_str();
  col_.append(&str, 1);
}

template <>void 
appender_ex<str16_t>::append(const string &s) {
  const char *str = s.c_str();
  short * val = str_dup<short, char>(str, strlen(str));
  col_.append(&val, 1);
  delete [] val;
}

template <>void
appender_ex<str32_t>::append(const string &s) {
  const char *str = s.c_str();
  int * val = str_dup<int, char>(str, strlen(str));
  col_.append(&val, 1);
  delete [] val;
}


template<typename T>T
read_float(const string & s) {
  T val;
  stringstream str(s);

  str >> val;

  if (str.fail()) {
    if (strcasecmp(s.c_str(), "nan") == 0) {
      val = numeric_limits<double>::quiet_NaN();
    } else if (strcasecmp(s.c_str(), "inf") == 0) {
      val = numeric_limits<double>::infinity();
    } else if (strcasecmp(s.c_str(), "-inf") == 0) {
      val = - numeric_limits<double>::infinity();
    } else {
      val = 0;
    }
  }

  return val;
}

template <>void 
appender_ex<float>::append(const string &s) {
  float val = read_float<float>(s);
  col_.append(&val, 1);
}

template <>void 
appender_ex<double>::append(const string &s) {
  double val = read_float<double>(s);
  col_.append(&val, 1);
}

appender::appender(column &col) : col_(col) {	
}

appender::~appender() {	
}

static appender *
create(column &col) {
  switch(col.type()) {
  case DS_T_BOOL:
    return new appender_ex<bool>(col);

  case DS_T_INT8:
    return new appender_ex<int8_t>(col);

  case DS_T_INT16:
    return new appender_ex<int16_t>(col);

  case DS_T_INT32:
    return new appender_ex<int32_t>(col);

  case DS_T_INT64:
    return new appender_ex<int64_t>(col);

  case DS_T_UINT8:
    return new appender_ex<uint8_t>(col);

  case DS_T_UINT16:
    return new appender_ex<uint16_t>(col);

  case DS_T_UINT32:
    return new appender_ex<uint32_t>(col);

  case DS_T_UINT64:
    return new appender_ex<uint64_t>(col);

  case DS_T_FLOAT32:
    return new appender_ex<float32_t>(col);

  case DS_T_FLOAT64:
    return new appender_ex<float64_t>(col);

  case DS_T_STRING8:
    return new appender_ex<str8_t>(col);

  case DS_T_STRING16:
    return new appender_ex<str16_t>(col);

  case DS_T_STRING32:
    return new appender_ex<str32_t>(col);

  case DS_T_INVALID:
  default:
    return NULL;

  }
}

static type_t str_to_type(const string &s) {
  if (s == "bool")
    return DS_T_BOOL;
  else if (s == "int8")
    return DS_T_INT8;
  else if (s == "int16")
    return DS_T_INT16;
  else if (s == "int32")
    return DS_T_INT32;
  else if (s == "int64")
    return DS_T_INT64;
  else if (s == "uint8")
    return DS_T_UINT8;
  else if (s == "uint16")
    return DS_T_UINT16;
  else if (s == "uint32")
    return DS_T_UINT32;
  else if (s == "uint64")
    return DS_T_UINT64;
  else if (s == "float32")
    return DS_T_FLOAT32;
  else if (s == "float64")
    return DS_T_FLOAT64;
  else if (s == "str8")
    return DS_T_STRING8;
  else if (s == "str16")
    return DS_T_STRING16;
  else if (s == "str32")
    return DS_T_STRING32;

  return DS_T_INVALID;
}
void usage(ostream &out, const string &prog)  {
  out << "Usage: " << prog << " options* input output" << endl
      << TAB << "input" << TAB "valid path to input csv file or '-' for stdin"  << endl
      << TAB << "output" << TAB "valid path to output data storage"  << endl
      << TAB << "Options:" << endl
      << TAB << TAB << "-f" << TAB << "Remove output storage if it exists" << endl
      << TAB << TAB << "-d<delimeter>" << TAB << "Use delimeter <delimeter>. Default is ','" << endl
      << TAB << TAB << "-q<quote>" << TAB << "Use quote <quote> for strings. Default quote is '\"' " << endl
      << TAB << TAB << "-n "  << TAB << "Read column names as a first line from csv" << endl
      << TAB << TAB << "-n<number> <name>"  << TAB << "Set name <name> for column #<number>" << endl
      << TAB << TAB << "-t<number> <type>"  << TAB << "Set type <type> for column #<number>" << endl
      << TAB << TAB << "-s<number>"  << TAB << "Set default string element size. Valid nnumber is 8, 16 or 32." << endl
      << TAB << TAB << "-g"  << TAB << "guess column types." << endl
      << TAB << TAB << "-h "  << TAB << "Prints this help and exit" << endl
      << endl
      << TAB << "Valid types are bool, int8, int16, int32, int64, uint8, uint16, uint32, uint64, float32, float64, str8, str16, str32. " << endl
      << TAB << TAB << "If no type provided, the programm will use str8 (for quoted entries) and float64 for all other by default" << endl
      << endl
      << TAB << "If no name provided, the programm will generate name as 'var_<number>'" << endl
      << endl
      << TAB << "Numeration started from 1" << endl
      << endl
      << TAB << "Example: csv2ds -n1 name -t1 str8 -n2 val -t2 int32 - /tmp/storage" << endl << endl;

}

int
parse_params(int argc, char **argv, options &opt) {
  opt.delim = ',';
  opt.quote = "\"";
  opt.head  = false;
  opt.force = false;
  opt.guess = false;
  opt.str_type = DS_T_STRING8;

  int file_count = 0;

  if (argc < 3) {
    cerr << endl << "Error: *** Not enough input arguments: expected at least two ***" << endl << endl;
    usage(cerr, argv[0]);
    return -1;
  }

  for (int k = 1; k < argc; ++ k) {
    string param = argv[k];
    if (param == "-" || param[0] != '-') {
      if (file_count == 0) {
        opt.input = param;
      } else if (file_count == 1) {
        opt.output = param;
      } else {
        cerr << endl << "Error: *** Invalid input parameters ***" << endl << endl;
        usage(cerr, argv[0]);
        return -1;
      }
      ++ file_count;
      continue;
    }

    if (param[1] == 'f') {
      opt.force = true;
      continue;
    }

    if (param[1] == 'g') {
      opt.guess = true;
      continue;
    }

    if (param[1] == 'd') {
      opt.delim = param.substr(2);
      continue;
    }

    if (param[1] == 'q') {
      opt.quote = param.substr(2);
      continue;
    }

    if (param[1] == 'h') {
      usage(cout, argv[0]);
      return 1;
    }

    if (param[1] == 's') {
      if (param == "-s8") {
        opt.str_type = DS_T_STRING8;
      } else if (param == "-s16") {
        opt.str_type = DS_T_STRING16;
      } else if (param == "-s32") {
        opt.str_type = DS_T_STRING32;
      } else {
        cerr << endl << "Error: *** -s8, -s16 or -s32. found " << param << " ***" << endl << endl;
        usage(cerr, argv[0]);
        return -1;
      }
      continue;
    }

    if (param[1] == 'n') {
      if (param.length() == 2) {
        opt.head = true;
        continue;
      }
      int num = 0;
      sscanf(param.c_str() + 2, "%d", &num);
      if (num == 0) {
        cerr << endl << "Error: *** expected -n<number>, while number > 0. found " << param << " ***" << endl << endl;
        usage(cerr, argv[0]);
        return -1;
      }

      if (k == argc - 1) {
        cerr << endl << "Error: *** Invalid input parameter " << argv[k] << endl << " ***"<< endl;
        usage(cerr, argv[0]);
        return -1;
      }

      if (opt.names.size() < num) {
        opt.names.resize(num);
      }
      opt.names[num - 1] = argv[++ k];
      continue;
    }

    if (param[1] == 't') {
      int num = 0;
      sscanf(param.c_str() + 2, "%d", &num);
      if (num == 0) {
        cerr << endl << "Error: *** expected -t<number>, while number > 0. found " << param << " ***" << endl << endl;
        usage(cerr, argv[0]);
        return -1;
      }

      if (k == argc - 1) {
        cerr << endl << "Error: *** Invalid input parameter " << argv[k] << " ***" << endl << endl;
        usage(cerr, argv[0]);
        return -1;
      }

      if (opt.types.size() < num) {
        opt.types.resize(num, DS_T_INVALID);
      }

      type_t t = str_to_type(argv[++ k]);

      if (t == DS_T_INVALID) {
        cerr << endl << "Error: *** Type " << argv[k] << " not recognized ***" << endl << endl;
        usage(cerr, argv[0]);
        return -1;
      }

      opt.types[num - 1] = t;
      continue;
    }

    cerr << endl << "Error: *** Invalid input parameter " << argv[k] << " ***" << endl << endl;
    usage(cerr, argv[0]);
    return -1;
  }

  if (opt.delim == "\\t") {
    opt.delim = TAB;
  }

  if (opt.input.empty()) {
    cerr << endl << "Error: *** Not specified input file ***" << endl << endl;
    usage(cerr, argv[0]);
    return -1;
  }

  if (opt.output.empty()) {
    cerr << endl << "Error: *** Not specified output file ***" << endl << endl;
    usage(cerr, argv[0]);
    return -1;
  }

  if (opt.guess && opt.input == "-") {
    cerr << endl << "Error: *** option '-g' (guess) conflicts with input file '-' ***" << endl << endl;
    usage(cerr, argv[0]);
    return -1;
  }

  return 0;
}

void
split(string &line, vector<string> &values, options &opt) {
  int pos = 0;
  while(true) {
    int next = line.find_first_of(opt.delim, pos);
    if (next < 0) {
      values.push_back(line.substr(pos));
      break;
    }

    values.push_back(line.substr(pos, next - pos));
    pos = next + 1;
  }
}

void 
remove_quote(const string &input, const string &quote, string &output) {
  if (input.length() >= quote.length() * 2 && input.substr(0, quote.length()) == quote && input.substr(input.length() - quote.length()) == quote) {
    output = input.substr(quote.length(), input.length() - quote.length() * 2);
  } else {
    output = input;
  }
}

void 
set_names(vector<string> &vals, options &opt) {
  for (size_t k = 0; k < vals.size(); ++ k) {
    if (opt.names[k].empty()) {
      remove_quote(vals[k], opt.quote, opt.names[k]);
    }
  }
}

type_t
guess_type(const string &input, const options &opt) {
  string word;
  remove_quote(input, opt.quote, word);
  if (word != input) {
    return opt.str_type;
  }

  if (strcasecmp(input.c_str(), "nan") == 0) {
    return DS_T_FLOAT64;
  }

  if (input == "0" || input == "1") {
    return DS_T_BOOL;
  }

  char *endp = NULL;
  const char * startp = input.c_str();

  long l = strtol(startp, &endp, 10);
  if (startp + input.length() == endp) {
    if (l <= numeric_limits<int8_t>::max() && l >= numeric_limits<int8_t>::min())
      return DS_T_INT8;
    else if (l <= numeric_limits<int16_t>::max() && l >= numeric_limits<int16_t>::min())
      return DS_T_INT16;
    else if (l <= numeric_limits<int32_t>::max() && l >= numeric_limits<int32_t>::min())
      return DS_T_INT32;
    else
      return DS_T_INT64;
  }

  strtod(startp, &endp);
  if (startp + input.length() == endp) {
    return DS_T_FLOAT64;
  }

  return opt.str_type;
}

void 
set_types(const vector<string> &vals, options &opt) {
  for (size_t k = 0; k < vals.size(); ++ k) {
    if (opt.types[k] == DS_T_INVALID) {
      type_t t = guess_type(vals[k], opt);
      if (is_str(t)) {
        opt.types[k] = opt.str_type;
      } else {
        opt.types[k] = DS_T_FLOAT64;
      }
    }
  }
}

void 
generate_columns(storage &ds, options &opt) {
  for (size_t k = 0; k < opt.types.size(); ++ k) {
    if (opt.types[k] == DS_T_INVALID) {
      throw runtime_error("column has type INVALID");
    }
  }

  for (size_t k = 0; k < opt.types.size(); ++ k) {
    if (is_str(opt.types[k])) {
      ds.add(opt.types[k], DS_T_UINT32, opt.names[k]);
    } else {
      ds.add(opt.types[k], opt.names[k]);
    }
  }

  ds.flush();
}

static bool 
get_values(istream &in, vector<string> &values, options &opt) {
  string line;
  getline(in, line);
  if (line.length() == 0) {
    return false;
  }

  values.clear();
  split(line, values, opt);

  return true;
}

void
convert(istream &in, storage &ds, options &opt) {
  in.precision(numeric_limits<double>::digits10);

  vector<string> values;
  int row = 0;
  if (!get_values(in, values, opt)) {
    return;
  }
  ++ row;

  size_t col_num = values.size();
  opt.names.resize(col_num);
  opt.types.resize(col_num, DS_T_INVALID);

  if (opt.head) {
    set_names(values, opt);

    if (!get_values(in, values, opt)) {
      generate_columns(ds, opt);
      return;
    }
    ++ row;
  }

  set_types(values, opt);
  generate_columns(ds, opt);

  vector<appender *> appenders;
  for (size_t k = 0; k < ds.cols(); ++ k) {
    appenders.push_back(create(ds[k]));
  }
  do {
    if (values.size() != col_num) {
      cerr << "Row #" << row << ": inconsistent column number - abort" << endl;
      break;
    }

    for (size_t k = 0; k < col_num; ++ k) {
      if (is_str(appenders[k] ->col().type())) {
        remove_quote(values[k], opt.quote, values[k]);
      }
      appenders[k] ->append(values[k]);
    }

  } while(!in.eof() && get_values(in, values, opt) && ++ row);
  cout << "read " << row << " rows" << endl;

  for (size_t k = 0; k < ds.cols(); ++ k) {
    delete appenders[k];
  }

  ds.flush();
}

void
guess_types(const vector<string> &vals, options &opt) {
  for (size_t k = 0; k < vals.size(); ++ k) {
    type_t t = guess_type(vals[k], opt);
    if (static_cast<int>(t) > static_cast<int>(opt.types[k])) {
      opt.types[k] = t;
    }
  }
}

void
guess(istream &in, options &opt)  {
  vector<string> values;
  int row = 0;
  if (!get_values(in, values, opt)) {
    return;
  }

  size_t col_num = values.size();
  opt.types.resize(col_num, DS_T_INVALID);

  if (opt.head) {
    if (!get_values(in, values, opt)) {
      return;
    }
  }

  guess_types(values, opt);

  do {
    if (values.size() != col_num) {
      cerr << "Row #" << row << ": inconsistent column number - abort" << endl;
      break;
    }

    guess_types(values, opt);

  } while(!in.eof() && get_values(in, values, opt));
}


int
main(int argc, char **argv) {
  options opt;
  int ret = parse_params(argc, argv, opt);
  if (ret < 0) {
    return ret;
  } else if (ret > 0) {
    return 0;
  }

  if (opt.guess) {
    ifstream in(opt.input.c_str());
    guess(in, opt);
  }
  try {
    int flags = DS_O_WRITE|DS_O_CREATE;
    if (opt.force) {
      flags |= DS_O_TRUNC;
    }

    storage stor(opt.output, flags);
    if (opt.input == "-") {
      convert(cin, stor, opt);
    } else {
      ifstream in(opt.input.c_str());
      convert(in, stor, opt);
    }
  } catch(exception &ex) {
    cerr << "Runtime Error: " << ex.what() << endl;
  }
}
