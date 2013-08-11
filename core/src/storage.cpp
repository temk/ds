#include <ds/storage.h>

#include <ds/column.h>
#include <ds/driver.h>
#include <ds/driver_dir.h>

#include <stdio.h>

#include <sstream>
#include <stdexcept>

using namespace ds;
using namespace std;

static const string INDEX("index");

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

storage::storage() 
 : col_num_(0), buff_siz_(0), driver_(NULL) {
}

storage::storage(const string &path, int mode, size_t buff_siz) 
 : col_num_(0), buff_siz_(buff_siz), driver_(0L) {	
	 open(path, mode, buff_siz);
}

storage::~storage() {
}

void 
storage::open(const string &path, int mode, size_t buff_siz) {
	driver *d = new driver_dir();
	try {
		d ->warn.set(warn.get());
		d ->info.set(info.get());		
		d ->open(path, mode);
	} catch(const runtime_error &ex) {
		delete d;
		throw ex;
	}
	
	driver_ = d;
	driver_ ->read_index(*this);	
}

void 
storage::flush() {
	if (!is_open()) {
		return;
	}
	
	for (col_list_t::iterator iter = col_by_index_.begin(); iter != col_by_index_.end(); ++ iter) {
		(*iter) ->flush();
	}	
	
	driver_ ->write_index(*this);
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
		err << "storage::column_at: No such column index: " << idx << endl;
	}
	return *col_by_index_[idx];
}

const column & 
storage::column_at(const string &name) const {
	col_map_t::const_iterator iter = col_by_name_.find(name);
	if (iter == col_by_name_.end()) {
		err << "storage::column_at: No such column name: " << name << endl;
	}
	return *(iter ->second);
}

column & 
storage::column_at(size_t idx) {
	if (idx >= col_by_index_.size()) {
		err << "storage::column_at: No such column index: " << idx << endl;
	}
	return *col_by_index_[idx];
}

column & 
storage::column_at(const string &name) {
	col_map_t::iterator iter = col_by_name_.find(name);
	if (iter == col_by_name_.end()) {
		err << "storage::column_at: No such column name: " << name << endl;
	}
	return *(iter ->second);
}

column &
storage::add(type_t type, const string &name, endian_t endian, ssize_t index) {	
	string var = find_free_name(name, col_by_name_);
	if (var != name) {
		warn << "storage::add: new column " << name << " got name " << var << endl;
	}
	
	column *col = new column(*this, type, type, var, endian);
	push(col, index);
	return *col;
}

column &
storage::add(type_t type, type_t dict, const string &name, endian_t endian, ssize_t index) {	
	string var = find_free_name(name, col_by_name_);
	if (var != name) {
		warn << "storage::add: new column " << name << " got name " << var << endl;
	}
	
	column *col = new column(*this, type, dict, var, endian);
	push(col, index);
	return *col;
}

size_t 
storage::index_of(const column *col) const {
	for (size_t k = 0; k < col_by_index_.size(); ++ k) {
		if (col_by_index_[k] == col) {
			return k;
		}
	}
	
	err << "storage::index_of: column '" << col -> name() << "' not found." << endl;
}


void 
storage::pop(column *col) {	
	col_by_index_.erase(col_by_index_.begin() + index_of(col));
	col_by_name_.erase(col ->name());
}

void 
storage::push(column *col, ssize_t index) {
	if (index == -1) {
		index = col_by_index_.size();
	}
	
	col_by_index_.insert(col_by_index_.begin() + index, col);
	col_by_name_[col ->name()] = col;
	
	++ col_num_;
}
