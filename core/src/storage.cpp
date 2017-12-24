#include <ds/version.h>
#include <ds/storage.h>

#include <ds/utils.h>
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
find_free_name(const string name, const map<string, column *> &map, bool unique = false) {
	string result = name;
	if (result.empty()) {
		result = "var";
	}
	if (map.count(result) > 0) {
        if (unique) {
            throw runtime_error("column already exists");
        }

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
 : col_num_(0), buff_siz_(0), driver_(NULL), mode_(0) {
}

storage::storage(const string &path, int mode, size_t buff_siz)
    : col_num_(0), buff_siz_(buff_siz), driver_(0L), mode_(0) {
	 open(path, mode, buff_siz);
}

storage::~storage() {
  close(); // just to be sure
}

void
storage::open(const string &path, int mode, size_t buff_siz) {
	driver *d = new driver_dir();
	try {
		d ->warn.set(warn.get());
		d ->info.set(info.get());
		d ->open(path, mode);
	} catch(const runtime_error &ex) {
	    try{
            d ->close();
            delete d;
	    } catch( ... ) {
	    }

		throw ex;
	}

    mode_ = mode;
	driver_ = d;

	try {
        driver_ ->read_index(*this);
	} catch(const exception &ex) {
	    try {
	        close();
	    } catch( ... ) {
	    }

	    throw ex;
	}
}

void
storage::flush() {
	if (!is_open()) {
		return;
	}

    if (mode_ & DS_O_WRITE) {
        for (col_list_t::iterator iter = col_by_index_.begin(); iter != col_by_index_.end(); ++ iter) {
		(*iter) ->flush();
        }

        driver_ ->write_index(*this);
    }
}

void
storage::close() {
	if (!is_open()) {
		return;
	}

    if (driver_ ->get_mode() & DS_O_WRITE) {
      flush();
    }

	driver_ ->close();
	delete driver_;
	driver_ = NULL;
    mode_ = 0;
}

bool
storage::is_open() const {
    return driver_ != NULL && mode_ != 0;
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
storage::add(type_t type, const string &name,  size_t width, endian_t endian, ssize_t index, bool compressed) {
	if (width < 1) {
		err << "storage::add: new column: width < 1 is illigal" << endl;
	}
	string var = find_free_name(name, col_by_name_, (mode_ & DS_O_UNIQUE) != 0);
	if (var != name) {
		warn << "storage::add: new column " << name << " got name " << var << endl;
	}

    column *col = new column(*this, type, type, var, width, endian, compressed);
    push(col, index);

    driver_ ->write_index(*this); // partial flush
	return *col;
}

column &
storage::add(type_t type, type_t dict, const string &name,  size_t width, endian_t endian, ssize_t index, bool compressed) {
	if (width < 1) {
		err << "storage::add: new column: width < 1 is illigal" << endl;
	}
	string var = find_free_name(name, col_by_name_, (mode_ & DS_O_UNIQUE) != 0);
	if (var != name) {
		warn << "storage::add: new column " << name << " got name " << var << endl;
	}

    column *col = new column(*this, type, dict, var, width, endian, compressed);
	push(col, index);

    driver_ ->write_index(*this); // partial flush
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

    -- col_num_;
    driver_ ->write_index(*this); // partial flush
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

void
storage::version(int &major, int &minor, int &build)
{
    major = MAJOR_VERSION;
    minor = MINOR_VERSION;
    build = BUILD_VERSION;
}

// =================================================================================================================================
void *ds_open(const char *path, int mode, int buff_siz) {
  return new ds::storage(path, mode, buff_siz);
}

size_t ds_cols(void *ds) {
  ds::storage *stor = reinterpret_cast<ds::storage *>(ds);
  return stor ->cols();
}

void * ds_add_column(void *ds, int c_type, int c_dict_type, const char *c_name, int width, int c_endian, int index ) {
  ds::storage *stor = reinterpret_cast<ds::storage *>(ds);

  string name = "";
  if (c_name != NULL) {
    name = c_name;
  }

  ds::type_t   type  = (ds::type_t)c_type;
  ds::type_t  dict_type  = (ds::type_t)c_dict_type;
  ds::endian_t endian = (ds::endian_t)c_endian;
  if (endian == DS_E_INVALID) {
    endian = DS_E_HOST;
  }

  if (ds::is_str(type) && (dict_type == DS_T_INVALID)) {
    dict_type = DS_T_UINT32;
  }

  ds::column & col = (dict_type == DS_T_INVALID)  ?
                      stor ->add(type, name, width, endian, index) :
                      stor ->add(type, dict_type, name, width, endian, index);

  return &col;
}

void * ds_get_column_by_name(void *ds, const char *name) {
  ds::storage *stor = reinterpret_cast<ds::storage *>(ds);
  return &(stor ->column_at(name));
}

void * ds_get_column_by_index(void *ds, int index) {
  ds::storage *stor = reinterpret_cast<ds::storage *>(ds);
  return &(stor ->column_at(index));
}

void ds_add_meta(void *ds, const char *key, const char *val) {
  ds::storage *stor = reinterpret_cast<ds::storage *>(ds);
  stor ->tags().set(key, val);
}

void ds_flush(void *ds) {
  ds::storage *stor = reinterpret_cast<ds::storage *>(ds);
  stor ->flush();
}

void   ds_close(void *ds) {
  ds::storage *stor = reinterpret_cast<ds::storage *>(ds);
  stor ->close();
  delete stor;
}

void ds_version(int *major, int *minor, int *build) {
	ds::storage::version(*major, *minor, *build);
}

// =================================================================================================================================
