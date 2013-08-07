#include <ds/column.h>
#include <ds/storage.h>

#include <ds/filter.h>
#include <ds/filter_str.h>
#include <ds/filter_buff.h>
#include <ds/filter_endian.h>
#include <ds/filter_driver.h>
#include <ds/driver.h>
#include <ds/utils.h>

#include <stdexcept>
#include <sstream>

using namespace ds;
using namespace std;

#define INDEX_KEY (name_ + ".idx")
#define DICT_KEY  (name_ + ".dic")
#define DATA_KEY  (name_ + ".dat")

static const char * ENDI_NAME [] = { "little", "big" };

static const char * TYPE_NAME [] = {
		"invalid", "bool", 
		"int8",   "int16",  "int32",  "int64", 
		"uint8", "uint16", "uint32", "uint64", 
		"float32", "float64", 
		"string8", "string16", "string32", 
		"blob"
};

column::column(storage &s, string name) 
	: storage_(s), driver_(s.driver_), name_(name), filter_(NULL) {	
		read_index();
		init_filters();
}

column::column(storage &s, type_t type, const string &name, int endian, int siz)
	: driver_(s.driver_), filter_(NULL), siz_(siz), endian_(endian), len_(0), dirty_(false), name_(name), type_(type), storage_(s) {

	if (siz == -1) {
		siz_ = size_of(type);
	}
	
	init_filters();
}

size_t 
column::index() const {
	return storage_.index_of(this);
}

column::~column(){
	delete filter_;
}

void 
column::remove(bool leave_on_disk) {
	flush();
	
	if (!leave_on_disk) {
		driver_ ->truncate(DATA_KEY,  0);
		driver_ ->truncate(DICT_KEY,  0);
		driver_ ->truncate(INDEX_KEY, 0);
	}
	storage_.remove(this);
}

column &
column::flush() {	
	if (dirty_) {
		filter_ ->flush();
		write_index();
	}
	return *this;
}

column &
column::truncate(size_t len) {
	if (dirty_) {
		filter_ ->flush();
	}
	
	dirty_ = true;
	driver_ ->truncate(DATA_KEY, len);
	return *this;
}

column &
column::append(const void *data, size_t num) {	
	dirty_ = true;
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

void 
column::read_index() {
	size_t len = driver_ ->length(INDEX_KEY);
	
	char *buff = new char [len];
	driver_ -> read(INDEX_KEY, 0, buff, len);
	
	char dot;
	string s_magic, s_end, s_name, s_type, s_endian, s_siz, s_len; 
	
	istringstream in(buff);
	delete [] buff;
	
	string val_type, val_endian;
	in  >> s_magic 
	    >> s_name   >> name_
		>> s_type   >> val_type
		>> s_endian >> val_endian
		>> s_siz    >> siz_
		>> s_len    >> len_
		>> s_end;
	   
	if (s_magic != "__DS__" || s_name != "name:"  || s_type != "type:" || s_endian != "endian:" || s_siz != "size:" || s_len != "length:") {
		throw runtime_error("Malformed column index");
	}
	
	for (int k = 0; k < sizeof(ENDI_NAME)/sizeof(ENDI_NAME[0]); ++ k) {
		if (val_endian == ENDI_NAME[k]) {
			endian_ = k; 
			break;
		}
	}
	
	for (int k = 0; k < sizeof(TYPE_NAME)/sizeof(TYPE_NAME[0]); ++ k) {
		if (val_type == TYPE_NAME[k]) {
			type_ = static_cast<type_t>(k); 
			break;
		}
	}
}

void 
column::write_index() const {
	string index = name_ + ".idx";
	
	const char *type_name = TYPE_NAME[type_];
	const char *endi_name = ENDI_NAME[endian_];
	
	ostringstream out;
	out << "__DS__"   << endl
	    << "name: "   << name_     << endl
		<< "type: "   << type_name << endl
		<< "endian: " << endi_name << endl
		<< "size: "   << siz_      << endl
		<< "length: " << len_      << endl
		<< "__END__" << endl;
	
	string data = out.str();
	storage_.driver_ ->write(index, data.c_str(), data.length(), false );
}
/**

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
**/