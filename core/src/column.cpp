#include <ds/column.h>
#include <ds/storage.h>

#include <ds/filter.h>
#include <ds/filter_str.h>
#include <ds/filter_buff.h>
#include <ds/filter_endian.h>
#include <ds/filter_driver.h>
#include <ds/utils.h>

#include <stdexcept>

using namespace ds;
using namespace std;

column::column(storage &s) 
	: storage_(s), filter_(NULL) {	
}

column::column(storage &s, type_t type, const string &name, int endian, int siz)
	: filter_(NULL), siz_(siz), endian_(endian), len_(0), name_(name), type_(type), storage_(s) {

	if (siz == -1) {
		siz_ = size_of(type);
	}
	
	init_filters();
}

size_t 
column::index() const {
	return 0;
}

column::~column(){
	delete filter_;
}

void 
column::remove(){
}

column &
column::flush() {	
	filter_ ->flush();
	return *this;
}

column &
column::truncate(size_t len) {
	return *this;
}

column &
column::append(const void *data, size_t num) {	
	filter_ ->put(data, num);
	len_ += num;
	return *this;
}

int 
column::read(size_t offset, size_t num, void *data) {	
	size_t rd = min(len_ - offset, num);
	filter_ ->get(offset, num, data);
	return rd;
}

int 
column::read(const void *indexes, int idx_siz, size_t num, void *data) {
	size_t rd = min(len_ , num);
	filter_ ->get(indexes, idx_siz, num, data);
	return rd;
}

void
column::push_filter(filter *f) {
	f ->next(filter_);
	filter_ = f;
}

void
column::init_filters() {
	type_t t_out = type_;
	if (type_ == DS_T_STRING8 || type_ == DS_T_STRING8 || type_ == DS_T_STRING32) {
		switch(siz_) {
		case 1:
			t_out = DS_T_UINT8;
			break;
			
		case 2:
			t_out = DS_T_UINT16;
			break;
			
		case 4:
			t_out = DS_T_UINT32;
			break;
			
		case 8:
			t_out = DS_T_UINT64;
			break;
			
		default:
			runtime_error("column::init_filters: Illigal element size");			
		}
	}	
	
	size_t buff_siz = storage_.buff_siz_ > 0  ? storage_.buff_siz_ : DS_BUFF_SIZ;
	
	push_filter(new filter_driver(t_out, siz_, name_ + ".dat", storage_.driver_));
	
	if (storage_.buff_siz_ > 0) {
		push_filter(new filter_buff(t_out, siz_, storage_.buff_siz_));
	}
	
	if (endian_ != DS_ENDIAN_HOST && siz_ > 1 && type_ != DS_T_BLOB) {
		push_filter(new filter_endian(t_out, siz_, buff_siz));
	}
	
	if (is_str(type_)) {
		push_filter(new filter_str(type_, t_out, buff_siz, name_ + ".dic", storage_.driver_));
	}
}

ostream& 
ds::operator<<(ostream &out, const column &col) {
	return out << "column: " 
	    << col.name_    << " "
		<< col.type_    << " "
		<< col.endian_  << " "
		<< col.siz_     << " "
		<< col.len_;
}

istream& 
ds::operator>>(istream &in, column &col) {
	string tmp;
	int type;
	in >> tmp 
 	   >> col.name_
	   >> type
	   >> col.endian_ 
	   >> col.siz_
	   >> col.len_;
	
	col.type_ = static_cast<type_t >(type);
	col.init_filters();
	return in;
}
