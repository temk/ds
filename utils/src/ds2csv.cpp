#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <stdexcept>

#include <ds/utils.h>
#include <ds/column.h>
#include <ds/storage.h>

using namespace ds;
using namespace std;

#define TAB "\t"

inline ostream &operator<<(ostream &out, uint8_t i) {
	return out << int(i);
}

inline ostream &operator<<(ostream &out, int8_t i) {
	return out << int(i);
}

inline ostream &operator<<(ostream &out, const short *s) {
	char *t = str_dup<char, short>(s, str_length(s));
	out << t;
	delete [] t;
	return out;
}

inline ostream &operator<<(ostream &out, const int *s) {
	char *t = str_dup<char, int>(s, str_length(s));
	out << t;
	delete [] t;
	return out;
}

struct options {
	bool names;
    bool force;
	string delim;
	string quote;
	string input;
	string output;
};

class printable_column {
protected:
	column &col_;
	string delim_;
	string quote_;

public:
	printable_column (column &col, const string & delim, const string & quote);
	
	virtual ~printable_column ();
	
	inline const column &col() const {
		return col_;
	}
	
	virtual void print(ostream &, size_t k) const = 0;
};

printable_column ::printable_column (column &col, const string & delim, const string & quote) 
    : col_(col), delim_(delim), quote_(quote) {	
}
	
printable_column ::~printable_column () {	
}


template<typename T>
class printable_column_ex : public printable_column { 
private:
	T *data_;
	
public:
	printable_column_ex(column &col, const string & delim, const string &quote = "") : printable_column (col, delim, quote) {
		data_ = new T[col.length()];
		col.read(0, col.length(), data_);
	}
	
	~printable_column_ex() {
		delete [] data_;
	}
	
	void print(ostream &out, size_t k) const {
		if (k < col_.length()) {
			out << quote_<< data_[k] << quote_;
		}
		out <<  delim_;
	}
};


printable_column * 
create(column &col, const string &delim, const string &quote) {
	switch(col.type()) {		
		case DS_T_BOOL:
			return new printable_column_ex<bool>(col, delim);
			
		case DS_T_INT8:
			return new printable_column_ex<int8_t>(col, delim);
			
		case DS_T_INT16:
			return new printable_column_ex<int16_t>(col, delim);
			
		case DS_T_INT32:
			return new printable_column_ex<int32_t>(col, delim);
			
		case DS_T_INT64:
			return new printable_column_ex<int64_t>(col, delim);
			
		
		case DS_T_UINT8:
			return new printable_column_ex<uint8_t>(col, delim);
			
		case DS_T_UINT16:
			return new printable_column_ex<uint16_t>(col, delim);
			
		case DS_T_UINT32:
			return new printable_column_ex<uint32_t>(col, delim);
			
		case DS_T_UINT64:
			return new printable_column_ex<uint64_t>(col, delim);
			
		
		case DS_T_FLOAT32:
			return new printable_column_ex<float>(col, delim);

		case DS_T_FLOAT64:
			return new printable_column_ex<double>(col, delim);
		
		case DS_T_STRING8:
			return new printable_column_ex<const char *>(col, delim, quote);
			
		case DS_T_STRING16:
			return new printable_column_ex<const short *>(col, delim, quote);
			
		case DS_T_STRING32:
			return new printable_column_ex<const int *>(col, delim, quote);
			
		case DS_T_INVALID: 
		default:
			throw runtime_error("Invalid column type");
	}
	return NULL;
}

void usage(ostream &out, const string &prog)  {
	out << "Usage: " << prog << "options* input output" << endl 
		<< TAB << "input" << TAB "valid path to data storage"  << endl
		<< TAB << "output" << TAB "valid path to output scv file or '-' for stdin"  << endl
		<< TAB << "Options:" << endl
        << TAB << TAB << "-f" << TAB << "Removes output file if it exists, befor write" << endl
        << TAB << TAB << "-d<delimeter>" << TAB << "Use delimeter <delimeter>. Default is ','" << endl
        << TAB << TAB << "-q<quote>" << TAB << "Use quote <quote> for strings. Default quote is '\"' " << endl
		<< TAB << TAB << "-n "  << TAB << "Write column names as a first line" << endl
		<< TAB << TAB << "-h "  << TAB << "Prints this help and exit" << endl
		<< endl << endl
		<< TAB << "Example: ds2csv -q\\' -n /tmp/storage -" << endl << endl;
}

int
parse_params(int argc, char **argv, options &opt) {
	opt.delim = ',';
	opt.quote = "\"";
	opt.names = false;
    opt.force = false;
	int file_count = 0;
	
	if (argc < 3) {
		usage(cerr, argv[0]);
		return false;
	}
	
	for (int k = 1; k < argc; ++ k) {
		string param = argv[k];
		if (param == "-" || param[0] != '-') {
			if (file_count == 0) {
				opt.input = param;
			} else if (file_count == 1) {
				opt.output = param;
			} else {
				usage(cerr, argv[0]);
				return -1;
			}
			++ file_count;
			continue;
		}
		
        if (param[1] == 'f') {
            opt.force = true;
        } else if (param[1] == 'd') {
            opt.delim = param.substr(2);
        } else if (param[1] == 'q') {
			opt.quote = param.substr(2);
		} else if (param[1] == 'n') {
			opt.names = true;
		} else if (param[1] == 'h') {
			usage(cout, argv[0]);
			return 1;
		} else {
			usage(cerr, argv[0]);
			return -1;
		}
	}
	
	if (opt.delim == "\\t") {
		opt.delim = "\t";
	}
		
	return 0;
}

void
convert(storage &stor, ostream &out, options &opt) {
	vector<printable_column *> cols;
	for (size_t k = 0; k < stor.cols() - 1; ++ k) {
		cols.push_back(create(stor[k], opt.delim, opt.quote));
	}
	cols.push_back(create(stor[stor.cols() - 1], "\n", opt.quote));
	
	if (opt.names) {
		for (size_t k = 0; k < cols.size(); ++ k) {
			out << (k > 0 ? opt.delim : "" ) << opt.quote << cols[k] ->col().name() << opt.quote ;
		}
		 out << endl;
	}
	
	size_t max_len = 0;
	for (size_t k = 0; k < cols.size(); ++ k)  {
		if (max_len < cols[k] ->col().length()) {
			max_len = cols[k] ->col().length();
		}
	}
	
	for (size_t n = 0; n < max_len; ++ n) {
		for (size_t k = 0; k < cols.size(); ++ k)  {
			cols[k] ->print(out, n);
		}					
	}
	
	for (size_t k = 0; k < cols.size(); ++ k) {
		delete cols[k];
	}
	 
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
	
	try {
		storage stor(opt.input, DS_O_READ);
		if (opt.output == "-") {
			convert(stor, cout, opt);
		} else {
            ios_base::openmode mode = ios_base::out;
            if (opt.force) {
                mode |= ios_base::trunc;
            } else {
                mode |= ios_base::app;
            }
            ofstream out(opt.output.c_str(), mode);
            convert(stor, out, opt);
		}
	} catch(exception &ex) {
		cerr << "Runtime Error: " << ex.what() << endl;
	}
}
