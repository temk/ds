#include <ds/column.h>
#include <ds/storage.h>

#include <ds/filter.h>
#include <ds/filter_str.h>
#include <ds/filter_buff.h>
#include <ds/filter_endian.h>
#include <ds/filter_driver.h>
#include <ds/driver.h>
#include <ds/utils.h>
#include <ds/lookup.h>

#include <stdlib.h>

#include <sstream>

using namespace ds;
using namespace std;


column::column(storage &s, string name) 
    : error_handler(s), dirty_(false), name_(name), width_(1), length_(0), endian_(DS_E_INVALID),
	int_type_(DS_T_INVALID), ext_type_(DS_T_INVALID), filter_(NULL), storage_(s) {
		
}

column::column(storage &s, type_t int_type, type_t ext_type, const string &name, size_t width, endian_t endian)
    : error_handler(s), dirty_(true), name_(name), width_(width), length_(0), endian_(endian),
	int_type_(int_type), ext_type_(ext_type), filter_(NULL), storage_(s) {

	init_filters();
}

void 
column::init(type_t int_type, type_t ext_type, size_t width, size_t length, endian_t endian) {
	int_type_ = int_type;
	ext_type_ = ext_type;
	endian_   = endian;
    width_    = width;
	length_   = length; 
	
	init_filters();
	
	storage_.push(this);
}

size_t 
column::index() const {
	return storage_.index_of(this);
}

column::~column(){
	delete filter_;
}

void 
column::remove() {
	storage_.driver_ ->remove(*this);
	storage_.pop(this);
}

column &
column::flush() {	
    if (dirty_) {
        filter_ ->flush();
    }
    storage_.driver_ ->write_index(*this);
    return *this;
}

column &
column::truncate(size_t len) {
	if (dirty_) {
		filter_ ->flush();
	}
	
	dirty_ = true;
    storage_.driver_ ->truncate(name_, len * width_);
    length_ = len;
    storage_.driver_ ->write_index(*this);
	return *this;
}

column &
column::append(const void *data, size_t num) {	
	dirty_ = true;
    filter_ ->put(data, num);
	length_ += num;
	return *this;
}

int 
column::read(size_t offset, size_t num, void *data) {	
	size_t rd = min(length_ - offset, num);
    filter_ ->get(offset, rd, data);
	return rd;
}

int 
column::read(const void *indexes, int idx_siz, size_t num, void *data) {
	size_t rd = min(length_ , num);
    filter_ ->get(indexes, idx_siz, num, data);
}

void
column::push_filter(filter *f) {
	f ->next(filter_);
	filter_ = f;
}

void
column::init_filters() {
	size_t buff_siz = storage_.buff_siz_ > 0  ? storage_.buff_siz_ : DS_BUFF_SIZ;
	
    push_filter(new filter_driver(*this, ext_type_, size_of(ext_type_), width_, name_, storage_.driver_));
	
	if (storage_.buff_siz_ > 0) {
        push_filter(new filter_buff(*this, ext_type_, size_of(ext_type_), width_, storage_.buff_siz_));
	}
	
	if (endian_ != DS_E_HOST && size_of(ext_type_) > 1) {
        push_filter(new filter_endian(*this, ext_type_, size_of(ext_type_), width_, buff_siz));
	}
	
	if (is_str(int_type_)) {
        push_filter(new filter_str(*this, int_type_, ext_type_, width_, buff_siz, name_, storage_.driver_, length_ > 0));
	}
	
	flush();
}

void 
column::set_string_accessor(string_accessor *acc) {
	if (is_str(int_type_)) {
		warn << "column::set_string_accessor: column " << name_ << " has non-string type " << int_type_ << ". Ignored.";
	}
	static_cast<filter_str *>(filter_) ->set_string_accessor(acc);
}


// ==============================================================================================
void ds_column_append(void *ptr, void *data, uint32_t len) {
  ds::column *col = reinterpret_cast<ds::column *>(ptr);
  col ->append(data, len);
}
