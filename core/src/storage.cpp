#include <ds/storage.h>

#include <ds/column.h>
#include <ds/driver.h>
#include <ds/driver_dir.h>

#include <stdio.h>

#include <sstream>
#include <stdexcept>

using namespace ds;
using namespace std;

static const string INDEX("index.idx");

static string 
find_free_name(const string name, const map<string, column *> &map) {
	string result = name;
	if (result.empty()) {
		result = "var";
	}
	if (map.count(result) > 0) {
		for (int k = 0; ; ++ k) {
			char buff[16];
			snprintf(buff, sizeof(buff), "%d", k);
			string str = result + "_" + string(buff);
			if (map.count(str) == 0) {
				result = str;
				break;
			}
		}	
	}
	return result;
}

storage::storage() {
}

storage::storage(const string &path, int mode, size_t buff_siz) 
 : col_num_(0), buff_siz_(buff_siz), driver_(0L) {	
	 open(path, mode, buff_siz);
}

storage::~storage() {
}

void 
storage::open(const string &path, int mode, size_t buff_siz) {
	try {
		driver_ = new driver_dir();
		driver_ ->open(path, mode);	
		
		if  (driver_ ->length(INDEX) > 0) {
			read_index();		
		}
		
	} catch(const runtime_error &ex) {
		driver_ ->close();
		delete driver_;
		driver_ = NULL;
		throw ex;
	}
}

void 
storage::flush() {
	if (!is_open()) {
		return;
	}
	
	for (col_list_t::iterator iter = col_by_index_.begin(); iter != col_by_index_.end(); ++ iter) {
		(*iter) ->flush();
	}
	
	write_index();
}

void 
storage::close() {
	if (!is_open()) {
		return;
	}
	
	flush();
	
	driver_ ->close();
	delete driver_;
	
	driver_ = NULL;
}

bool 
storage::is_open() const {
	return driver_ != NULL;
}

const column & 
storage::column_at(size_t idx) const {
	if (idx >= col_by_index_.size()) {
		throw runtime_error("No such column index");
	}
	return *col_by_index_[idx];
}

const column & 
storage::column_at(const string &name) const {
	col_map_t::const_iterator iter = col_by_name_.find(name);
	if (iter == col_by_name_.end()) {
		throw runtime_error("No such column: " + name);
	}
	return *(iter ->second);
}

column & 
storage::column_at(size_t idx) {
	if (idx >= col_by_index_.size()) {
		throw runtime_error("No such column index");
	}
	return *col_by_index_[idx];
}

column & 
storage::column_at(const string &name) {
	col_map_t::iterator iter = col_by_name_.find(name);
	if (iter == col_by_name_.end()) {
		throw runtime_error("No such column: " + name);
	}
	return *(iter ->second);
}

column &
storage::add(type_t type, const string &name, int endian, size_t index, int siz) {	
	string var = find_free_name(name, col_by_name_);
	column *col = new column(*this, type, var, endian, siz);
	add_column(col, index);
	return *col;
}

void 
storage::read_index() {
	size_t len = driver_ ->length(INDEX);
	if (len == 0) {
		return;		
	}
	
	char *buff = new char [len];
	driver_ -> read(INDEX, 0, buff, len);
	
	char dot;
	size_t major,minor, col_num;
	string magic, version, rows, cols; 
	
	istringstream in(buff);
	delete [] buff;
	
	in >> magic 
	   >> version >> major >> dot >> minor 
	   >> cols >> col_num;
	   
	if (magic != "__DS__" || version != "version:" || cols != "cols:" || dot != '.') {
		throw runtime_error("Malformed index");
	}
	
	if (major > MAJOR_VERSION || (major == MAJOR_VERSION && minor > MINOR_VERSION)) {
		throw runtime_error("versioning problem");
	}
	
	for (size_t k = 0; k < col_num; ++ k) {
		column *col = new column(*this);		
		in >> *col;
		
		add_column(col);
	}
	
	in >> magic; //todo: check __END__
}

void 
storage::write_index() const {
	ostringstream out;
	out << "__DS__"    << endl
	    << "version: " << MAJOR_VERSION << "." << MINOR_VERSION << endl
		<< "cols: "    << col_num_ << endl;
		
	for (col_list_t::const_iterator iter = col_by_index_.begin(); iter != col_by_index_.end(); ++ iter)	{
		out << *(*iter) << endl;
	}
	out << "__END__" << endl;
	
	string data = out.str();
	driver_ ->write(INDEX, data.c_str(), data.length(), false );
}

void 
storage::add_column(column *col, size_t i) {
	if (i == size_t(-1)) {
		i = col_by_index_.size();
	}
	
	col_by_index_.insert(col_by_index_.begin() + i, col);
	col_by_name_[col ->name()] = col;
	
	++ col_num_;
}
