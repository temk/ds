#include <ds/driver_dir.h>
#include <ds/storage.h>

#include <stdexcept>

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
			throw runtime_error("Unexpected index size");
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
		throw runtime_error("driver_dir::construct_fullpath: parent folder not exists or not available");
	}
	
	fullpath = string(tmp) + PATH_SEPARATOR + name;
}

void 
driver_dir::remove_all() const {
	string filename;
	
	struct dirent *entry = NULL;
	DIR *dir = opendir(base_.c_str());
	while((entry = readdir(dir)) != NULL) {
		construct_filename(entry ->d_name, filename);
		remove(filename.c_str());
	}
}

void 
driver_dir::open(const string &base, int mode){
	construct_fullpath(base, base_);

	struct stat status;
	int ret = stat(base_.c_str(), &status);
	
	if (ret == -1 && errno == ENOENT) {
		
		if ((mode & DS_O_CREATE) == 0) {
			throw runtime_error("driver_dir::open: Directory not found");
		}
		
		if (mkdir(base_.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
			throw runtime_error("driver_dir::open: Cannot create directory");
		}
		
	} else if (S_ISDIR(status.st_mode) == 0) {
		throw runtime_error("driver_dir::open: File is not directory");
	}

	if (mode & DS_O_TRUNC) {
		remove_all();
	}
	
	mode_ = mode;
}

void 
driver_dir::close() {	
	mode_ = 0;
}
		
void 
driver_dir::write(const string &key, const void *data, size_t len, bool append) {	
	if ((mode_ & DS_O_WRITE) == 0) {
		throw runtime_error("driver_dir::write: Not openned for write");
	}
	
	string filename;
	construct_filename(key, filename);

	int mask = O_WRONLY|O_CREAT;
	if (append) {
		mask |= O_APPEND;
	}
	
	int fd = ::open(filename.c_str(), mask, 0600);
	
	if (fd == -1) {
		throw runtime_error("driver_dir::write: Cannot open file for write");
	}
	
	
	ssize_t wr = ::write(fd, data, len);
	::close(fd);
	
	if (wr != len) {
		throw runtime_error("Error during write");
	}
}

void 
driver_dir::read(const string &key, size_t offs, void *dst, size_t len) {	
	if ((mode_ & DS_O_READ) == 0) {
		throw runtime_error("Not openned for read");
	}
	
	string filename;
	construct_filename(key, filename);
	
	int fd = ::open(filename.c_str(), O_RDONLY);
	
	if (fd == -1) {
		throw runtime_error("Cannot open file for read");
	}

	::lseek(fd, offs, SEEK_SET);
	ssize_t rd = ::read(fd, dst, len);
	::close(fd);
	
	if (rd != len) {
		throw runtime_error("Error during read");
	}	
}

void 
driver_dir::read(const string &key, const void *indexes, int idx_siz, size_t num, void *dst1, size_t el_siz) {	
	if ((mode_ & DS_O_READ) == 0) {
		throw runtime_error("Not openned for read");
	}
	
	char * dst = static_cast<char *>(dst1);
	string filename;
	construct_filename(key, filename);
	
	int fd = ::open(filename.c_str(), O_RDONLY);
	
	if (fd == -1) {
		throw runtime_error("Cannot open file for read");
	}

	accessor_t acc = get_accessor(idx_siz);
	
	for (int k = 0; k < num; ++ k) {
		//cout << "el siz: " << el_siz << ": get " << acc(indexes, k) << " with offset " << (acc(indexes, k) * el_siz) << endl;
		::lseek(fd, acc(indexes, k) * el_siz, SEEK_SET);
		if (::read(fd, dst, el_siz) != el_siz) {
			::close(fd);
			throw runtime_error("Error during read file");
		}
		
		dst += el_siz;
	}
	::close(fd);
}
	
/*	
void 
driver_dir::write_index(const string &text) {
	cout << "write index" << endl;
	if ((mode_ & DS_O_WRITE) == 0) {
		throw runtime_error("Not openned for write");
	}
	
	path filename = base_;
	filename /= "index.dat";
	
	int fd = ::open(filename.c_str(), O_WRONLY|O_TRUNC|O_CREAT, 0600);
	
	if (fd == -1) {
		throw runtime_error(string("Cannot open index for write: ") + filename.c_str());
	}
	
	ssize_t wr = ::write(fd, text.c_str(), text.length());
	::close(fd);
	
	if (wr != text.length()) {
		throw runtime_error("Error during write");
	}
}

void 
driver_dir::read_index(string &text) {	
	if ((mode_ & DS_O_READ) == 0) {
		throw runtime_error("Not openned for read");
	}
	
	path filename = base_;
	filename /= "index.dat";
	
	ifstream in(filename.string().c_str());
	if (!in.is_open()) {
		throw runtime_error("cannot read index file");
	}
	std::string contents((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
	text = contents;
}
*/

size_t
driver_dir::length(const string &key) {
	string filename;
	construct_filename(key, filename);

	struct stat status;
	int ret = stat(filename.c_str(), &status);
	if (ret == -1 && errno == ENOENT) {
		return 0;
	}
	return status.st_size;
}

void 
driver_dir::truncate(const string &key, size_t sz) {
	string filename;
	construct_filename(key, filename);

	if (sz == 0) {
		unlink(filename.c_str());
	} else {
		truncate(filename.c_str(), sz);
	}
}

/*
bool 
driver_dir::has_index() {
	if (mode_ == 0) {
		throw runtime_error("Not openned");
	}
	
	path filename = base_;
	filename /= "index.dat";
	return exists(filename);
}*/