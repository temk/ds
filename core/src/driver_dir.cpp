#include <ds/driver_dir.h>
#include <ds/storage.h>
#include <ds/column.h>
#include <ds/lookup.h>
#include <ds/utils.h>

#include <fstream>
#include <fcntl.h>
#include <dirent.h>
#include <unistd.h>
#include <errno.h>
#include <limits.h>
#include <stdlib.h>

#include <sys/stat.h>
#include <sys/types.h>

using namespace ds;
using namespace std;

#if defined(WIN32) 
#	define PATH_SEPARATOR '\\'
#else
#	define PATH_SEPARATOR '/'
#endif // PATH_SEPARATOR

#define FILE_MODE (S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH)


template<typename T>void
driver_dir::read_field(istream &in, const string &magic, T *val) {
	string temp;
	in >> temp;
	if (temp != magic) {
		err << "driver_dir::read_field: malformed index: expected " << magic << " but found " << temp << endl;
	}
	
	if (val) {
		in >> *val;
	}	
}

template<typename T>
size_t get_index(const void *data, size_t k) {
	return static_cast<const T*>(data)[k];
}

typedef size_t (*accessor_t)(const void *, size_t);
 
 accessor_t get_accessor(int siz) {
	switch(siz) {
		case 1: return &get_index<unsigned char>;		
		case 2: return &get_index<unsigned short>;		
		case 4: return &get_index<unsigned int>;		
		case 8: return &get_index<unsigned long>;		
		default:
			return NULL;
	}
}

driver_dir::driver_dir() 
	: mode_(0) {
}

driver_dir::~driver_dir() {	
}
		
void
driver_dir::construct_filename(const string &key, string &result) const {
	result = base_ + PATH_SEPARATOR + key.c_str();
}

void 
driver_dir::construct_fullpath(const string &path, string &fullpath) const {
	string parent = ".";
	string name = path;
	
	string::size_type pos = path.rfind(PATH_SEPARATOR);
	if (pos != string::npos) {
		parent = path.substr(0, pos);
		name   = path.substr(pos + 1);
	}
	
	char tmp[PATH_MAX];
	const char *p = realpath(parent.c_str(), tmp);
	if (p == NULL) {
		err << "driver_dir::construct_fullpath: parent folder '" 
		    << parent
			<< "' not exists or not available."
			<< endl;
	}
	
	fullpath = string(tmp) + PATH_SEPARATOR + name;
}

size_t
driver_dir::filesize(const string &filename) const {
	struct stat status;
	int ret = stat(filename.c_str(), &status);
	if (ret == -1 && errno == ENOENT) {
		return size_t(-1);
	}
	return status.st_size;	
}

void
driver_dir::create_empty_index() {
	string index;
	construct_filename("index", index);
	ofstream out(index.c_str());
	out << "__DS__"    << endl
	<< "version: " << MAJOR_VERSION << "." << MINOR_VERSION << endl
	<< "__END__" << endl;
}

void 
driver_dir::remove_all() const {
	string filename;
	
	struct dirent *entry = NULL;
	DIR *dir = opendir(base_.c_str());
	while((entry = readdir(dir)) != NULL) {
		construct_filename(entry ->d_name, filename);
		::remove(filename.c_str());
	}
}

void 
driver_dir::open(const string &base, int mode) {
	construct_fullpath(base, base_);

	struct stat status;
	int ret = stat(base_.c_str(), &status);
	
	if (ret == -1 && errno == ENOENT) {		
		if ((mode & DS_O_CREATE) == 0) {
			err << "driver_dir::open: Directory '" << base_ << "' not found" << endl;
		}
		
		if (mkdir(base_.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
			err << "driver_dir::open: Cannot create directory '" << base_  << "'." << endl;
		}
		
		create_empty_index();
		
	} else if (S_ISDIR(status.st_mode) == 0) {
		err << "driver_dir::open: File '" << base_ << "' is not directory" << endl;
	} 

	if (mode & DS_O_TRUNC) {
		remove_all();
		create_empty_index();
	}
	
	mode_ = mode;
}

void 
driver_dir::close() {	
	mode_ = 0;
}
		
void 
driver_dir::write(const string &key, const void *data, size_t len) {	
	if ((mode_ & DS_O_WRITE) == 0) {
		err << "driver_dir::write: Not openned for write." << endl;
	}
	
	string filename;
	construct_filename(key + ".dat", filename);

	int mask = O_WRONLY|O_CREAT|O_APPEND;
	int fd = ::open(filename.c_str(), mask, FILE_MODE);
	
	if (fd == -1) {
		err << "driver_dir::write: Cannot open file '" << filename << "'for write." << endl;
	}
	
	
	ssize_t wr = ::write(fd, data, len);
	::close(fd);
	
	if (wr != len) {
		err << "Error during write file '" << filename << "'." << endl;
	}
}

void 
driver_dir::read(const string &key, size_t offs, void *dst, size_t len) {	
	if ((mode_ & DS_O_READ) == 0) {
		err << "driver_dir::read: Not openned for read." << endl;
	}
	
	string filename;
	construct_filename(key + ".dat", filename);
	
	int fd = ::open(filename.c_str(), O_RDONLY);
	
	if (fd == -1) {
		err << "Cannot open file '" << filename << "'for read." << endl;
	}

	::lseek(fd, offs, SEEK_SET);
	ssize_t rd = ::read(fd, dst, len);
	::close(fd);
	
	if (rd != len) {
		err << "Error during read file '" << filename << "'." << endl;
	}	
}

void 
driver_dir::read(const string &key, const void *indexes, int idx_siz, size_t num, void *dst1, size_t el_siz) {	
	if ((mode_ & DS_O_READ) == 0) {
		err << "driver_dir::read: Not openned for read." << endl;
	}
	
	char * dst = static_cast<char *>(dst1);
	string filename;
	construct_filename(key + ".dat", filename);
	
	int fd = ::open(filename.c_str(), O_RDONLY);
	
	if (fd == -1) {
		err << "Cannot open file '" << filename << "' for read." << endl;
	}

	accessor_t acc = get_accessor(idx_siz);
	
	for (int k = 0; k < num; ++ k) {
		::lseek(fd, acc(indexes, k) * el_siz, SEEK_SET);
		if (::read(fd, dst, el_siz) != el_siz) {
			::close(fd);
			err << "Error during read file '" << filename << "'." << endl;
		}
		
		dst += el_siz;
	}
	::close(fd);
}

void 
driver_dir::truncate(const string &key, size_t sz) {
	string filename;
	construct_filename(key + ".dat", filename);
	
	::truncate(filename.c_str(), sz);
}

void 
driver_dir::remove(const column &col) {
	
}

void 
driver_dir::write_dictionary(const lookup &l) {
	string filename;
	construct_filename(l.key() + ".dic", filename);

	int el_siz = 0;
    size_t count = l.count();
	void **data = new void *[count];
	size_t *len  = new size_t[count];
	l.get(data, len, el_siz);
	
	int fd = ::open(filename.c_str(), O_WRONLY|O_TRUNC|O_CREAT, FILE_MODE);
	
	if (fd == -1) {
		err << "driver_dir::write_dictionary: Cannot open file '" << filename << "' for write." << endl;
	}

	for (size_t k = 0; k < count; ++ k) {
		size_t bytes = (len[k] + 1)* el_siz;
		if (::write(fd, data[k], bytes) != bytes) {
			delete [] len;
			delete [] data;
			::close(fd);
			err << "Error during write file '" << filename << "'." << endl;
		}
	}
	
	::close(fd);
	
	delete [] len;
	delete [] data;
}

void 
driver_dir::write_index(const storage &stor) {
	string index;
	construct_filename("index", index);
	
	ofstream out(index.c_str());
	if (!out) {
		err << "driver_dir::write_index: Can't open file '" << index << "' for write." << endl;
	}
	
	out << "__DS__"    << endl
	    << "version: " << MAJOR_VERSION << "." << MINOR_VERSION << endl;
		
	for (size_t k = 0; k < stor.cols(); ++ k)	{
		out << "column: " << stor[k].name() << endl;
	}
	
    vector<string> keys;
    stor.tags().keys(keys);
    for (size_t k = 0; k < keys.size(); ++ k) {
        out << "meta: \"" << keys[k] << "\" \"" <<  stor.tags().get(keys[k]) << "\"" << endl;
    }
	out << "__END__" << endl;
}

void 
driver_dir::write_index(const column &col) {
	string index;
	construct_filename(col.name() + ".idx", index);
	
	ofstream out(index.c_str());
	if (!out) {
		err << "driver_dir::write_index: Can't open file '" << index << "' for write." << endl;
	}
	
	out << "__DS__"   << endl
	    << "name: "   << col.name()    << endl
		<< "type: "   << col.type()    << endl;

	if (is_str(col.type())) {
		out << "dict: "   << col.ext_type() << endl;
	}
		
	out << "endian: " << col.endian()  << endl
        << "length: " << col.length()  << endl;

    vector<string> keys;
    col.tags().keys(keys);
    for (size_t k = 0; k < keys.size(); ++ k) {
        out << "meta: \"" << keys[k] << "\" \"" <<  col.tags().get(keys[k]) << "\"" << endl;
    }

    out << "__END__" << endl;
}

void 
driver_dir::read_dictionary(lookup &l) {
	string filename;
	construct_filename(l.key() + ".dic", filename);
	
	ssize_t sz = filesize(filename);

	int fd = ::open(filename.c_str(), O_RDONLY);
	
	if (fd == -1) {
		err << "driver_dir::read_dictionary: Cannot open file '" << filename << "' for read." << endl;
	}
	
	char *data = new char[sz];
	
	int rd = ::read(fd, data, sz);
	::close(fd);

	if (rd == sz) {
		l.set(data, rd);
    }
	delete [] data;	
	
	if (rd != sz) {
		err << "driver_dir::read_dictionary: error during read file '" << filename << "'." << endl;
	}
	
}

void 
driver_dir::read_index(storage &stor) {
	string index;
	construct_filename("index", index);
	
	ifstream in(index.c_str());
	if (!in) {
		err << "driver_dir::read_index: Can't open file '" << index << "' for read." << endl;
	}

	char dot;
	int minor, major;
	read_field(in, "__DS__",   (char *)NULL);
	read_field(in, "version:", &major);
	in >> dot >> minor;
	
	if (major > MAJOR_VERSION || (major == MAJOR_VERSION && minor > MINOR_VERSION)) {
		err << "driver_dir::read_index: cannot read version higher then " 
		    << MAJOR_VERSION << "." << MINOR_VERSION
			<< " but found " << major << "." << minor << endl;
	}
	
	string col_magic, col_val;
	while( true ) {
		in >> col_magic;
		if (col_magic == "column:") {
			in >> col_val;
			try {
				column *col = new column(stor, col_val);
				read_index(*col);
			} catch (exception &ex) {
				warn << "driver_dir::read_index: " << ex.what() << endl;
				warn << "driver_dir::read_index: column " << col_val << " ignored." << endl;
			}
        } else if (col_magic == "meta:") {
            string key, val;
            in >> key >> val;
            stor.tags().set(key, val);
        } else if (col_magic != "__END__") {
            warn << "driver_dir::read_index: expected 'column:', but found '" << col_magic << "'. Abort." << endl;
			break;
		} else {
			break;
		}
	}
}

void 
driver_dir::read_index(column &col) {
	string index;
	construct_filename(col.name() + ".idx", index);
	
	ifstream in(index.c_str());
	if (!in) {
		err << "driver_dir::read_index: Can't open file '" << index << "' for read." << endl;
	}
	
	string name;
	size_t length;
	endian_t endian;
	type_t int_type, ext_type;
	
	read_field(in, "__DS__",  (char *)NULL);
	read_field(in, "name:",   &name);
	read_field(in, "type:",   &int_type);
	if (is_str(int_type)) {
		read_field(in, "dict:",   &ext_type);
	} else {
		ext_type = int_type;
	}
	read_field(in, "endian:", &endian);
	read_field(in, "length:", &length);
	read_field(in, "__END__", (char *)NULL);
	
	if (name != col.name()) {
		warn << "driver_dir::read_index: column '" << col.name() << "' had different name '" << name << "'" << endl;
	}
	
	col.init( int_type, ext_type, length, endian);
}
