#include <csv2ds.h>
using namespace ds;
using namespace csv2ds;

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
using namespace std;

#define TAB '\t'

static type_t
str_to_type(const string &s) {
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

void
csv2ds::usage(ostream &out, const string &prog)  {
  out << "Usage: " << prog << " options* input output" << endl
      << TAB << "input" << TAB << "valid path to input csv file or '-' for stdin"  << endl
      << TAB << "output" << TAB << "valid path to output data storage"  << endl
      << TAB << "Options:" << endl
      << TAB << TAB << "-f" << TAB << "Remove output storage if it exists" << endl
      << TAB << TAB << "-d<delimeter>" << TAB << "Use delimeter <delimeter>. Default is ','" << endl
      << TAB << TAB << "-q<quote>" << TAB << "Use quote <quote> for strings. Default quote is 'no quote' (i.e. -q0) " << endl
      << TAB << TAB << "-n "  << TAB << "Read column names as a first line from csv" << endl
      << TAB << TAB << "-n<number> <name>"  << TAB << "Set name <name> for column #<number>" << endl
      << TAB << TAB << "-t<number> <type>"  << TAB << "Set type <type> for column #<number>" << endl
      << TAB << TAB << "-t<name> <type>"  << TAB << "Set type <type> for column named <name>" << endl
      << TAB << TAB << "-s<number>"  << TAB << "Set default string element size. Valid nnumber is 8, 16 or 32." << endl
      << TAB << TAB << "-[Gg]<number>"  << TAB << "guess column types based on <number> first rows." << endl
      << TAB << TAB << "-c<number>"  << TAB << "process only <number> first columns" << endl
      << TAB << TAB << "-m<number> <key> <value>"  << TAB << "add user defined metadata to column" << endl
      << TAB << TAB << "-m<name> <key> <value>"  << TAB << "add user defined metadata to column" << endl
      << TAB << TAB << "-m <key> <value>"  << TAB << "add user defined metadata to data storage" << endl
      //<< TAB << TAB << "-w<name> <number>"  << TAB << "set width for column" << endl
      //<< TAB << TAB << "-w<number> <number>"  << TAB << "set width for column" << endl
      << TAB << TAB << TAB << "For '-g' options, the type will choosen betwenn string or float64. If '-G' specified, then more tightly type guessing" << endl
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
csv2ds::parse_params(int argc, char **argv, options &opt) {
  opt.delim = ',';
  opt.quote = '\0';
  opt.head  = false;
  opt.force = false;
  opt.guess_rows = 1;
  opt.guess_tight = false;
  opt.col_limit = 0;
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

    if (param[1] == 'g' || param[1] == 'G') {
      sscanf(param.c_str() + 2, "%d", &opt.guess_rows);
      if (opt.guess_rows == 0) {
        cerr << endl << "Error: *** expected -" << param[1] <<"<number>, while number > 0. found " << param << " ***" << endl << endl;
        usage(cerr, argv[0]);
        return -1;
      }
      opt.guess_tight = param[1] == 'G';
      continue;
    }

    if (param[1] == 'c') {
      sscanf(param.c_str() + 2, "%d", &opt.col_limit);
      if (opt.col_limit == 0 && param != "-c0") {
        cerr << endl << "Error: *** expected -c<number>. found " << param << " ***" << endl << endl;
        usage(cerr, argv[0]);
        return -1;
      }
      continue;
    }

    if (param[1] == 'd') {
      if (param == "-d' '") {
        opt.delim = ' ';
      } else if (param == "-d\\t") {
        opt.delim = '\t';
      } else {
        opt.delim = param[2];
      }
      continue;
    }

    if (param[1] == 'q') {
      if (param.length() == 3) {
        opt.quote = param[2];
      }
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

    if (param[1] == 'm') {
      char *endp = NULL;
      long num = strtol(param.c_str() + 2, &endp, 10);

      if (k + 2 >= argc) {
        cerr << endl << "Error: *** Invalid input parameter " << argv[k] << " ***" << endl << endl;
        usage(cerr, argv[0]);
        return -1;
      }

      struct user_meta m;
      m.global = param.length() == 2;
      m.key = argv[++ k];
      m.val = argv[++ k];

      if (!m.global) {
        if (endp != param.c_str() + param.length()) {
          m.col_name = param.substr(2);
          m.col_num  = -1;
        } else {
          m.col_num  = num;
        }
      }

      opt.meta.push_back(m);
      continue;
    }

    if (param[1] == 't') {
      char *endp = NULL;
      long num = strtol(param.c_str() + 2, &endp, 10);

      if (k == argc - 1) {
        cerr << endl << "Error: *** Invalid input parameter " << argv[k] << " ***" << endl << endl;
        usage(cerr, argv[0]);
        return -1;
      }

      type_t t = str_to_type(argv[++ k]);

      if (t == DS_T_INVALID) {
        cerr << endl << "Error: *** Type " << argv[k] << " not recognized ***" << endl << endl;
        usage(cerr, argv[0]);
        return -1;
      }

      if (endp != param.c_str() + param.length()) {
        opt.name_with_type.push_back(make_pair(param.substr(2), t));
        continue;
      }

      if (opt.types.size() < num) {
        opt.types.resize(num, DS_T_INVALID);
      }

      opt.types[num - 1] = t;
      continue;
    }

/*
    if (param[1] == 'w') {
      char *endp = NULL;
      long num = strtol(param.c_str() + 2, &endp, 10);

      if (k == argc - 1) {
        cerr << endl << "Error: *** Invalid input parameter " << argv[k] << " ***" << endl << endl;
        usage(cerr, argv[0]);
        return -1;
      }

      long w = atol(argv[++ k]);


      if (endp != param.c_str() + param.length()) {
        opt.name_with_width.push_back(make_pair(param.substr(2), w));
        continue;
      }

      if (opt.width.size() < num) {
        opt.width.resize(num, 1);
      }

      opt.width[num - 1] = w;
      continue;
    }
*/
    cerr << endl << "Error: *** Invalid input parameter " << argv[k] << " ***" << endl << endl;
    usage(cerr, argv[0]);
    return -1;
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

  return 0;
}
