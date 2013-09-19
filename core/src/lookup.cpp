#include <ds/utils.h>
#include <ds/lookup.h>
#include <ds/strings.h>

#include <string.h>

#include <map>
#include <vector>
#include <limits>
#include <utility>

#define KEY(t1, t2) ((t1 << 8) | t2)

namespace ds {
	struct string_less : binary_function <string_container,string_container,bool> {
		
		inline bool operator() (const string_container& x, const string_container& y) const {
			if (x.len != y.len) return x.len < y.len;
			if (x.siz == y.siz) return memcmp(x.str, y.str, x.len * x.siz) < 0;
			
			switch(x.siz) {
				case sizeof(char): {
					switch(y.siz) {
						case sizeof(short): return str_less(static_cast<const char*>(x.str), static_cast<const short*>(y.str), x.len);
						case sizeof(int):   return str_less(static_cast<const char*>(x.str), static_cast<const int*>(y.str), x.len);
					}
				}
				
				case sizeof(short): {
					switch(y.siz) {
						case sizeof(char):  return str_less(static_cast<const short*>(x.str), static_cast<const char*>(y.str), x.len);
						case sizeof(int):   return str_less(static_cast<const short*>(x.str), static_cast<const int*>(y.str), x.len);
					}
				}
				
				case sizeof(int): {
					switch(y.siz) {
						case sizeof(char):  return str_less(static_cast<const int*>(x.str), static_cast<const char*>(y.str), x.len);
						case sizeof(short): return str_less(static_cast<const int*>(x.str), static_cast<const short*>(y.str), x.len);
					}
				}
				
			}
			return 0;
		}
	};

	template<typename T>
	class default_string_accessor : public string_accessor {
	public:	
        void get(size_t k, const void *, string_container &);
        void set(size_t k, const string_container &, void * );
	};
	
	template<typename T, typename N>
	class lookup_impl : public lookup {
	private:
		typedef string_container str_t;
		typedef vector<str_t> idx_to_str_t;
		typedef map<str_t, N,  string_less> str_to_idx_t;		
		
		string key_;
		str_to_idx_t str_to_idx_;
		idx_to_str_t idx_to_str_;
		
		default_string_accessor<T> default_string_accessor_;
        string_accessor * string_accessor_;
		
	public:	
		lookup_impl(const string &);
		
		void lookup_index(const void *str, void *val, size_t from, size_t num);
		void lookup_value(const void *val, void *str, size_t from, size_t num);

		const string &key() const;
        void set_string_accessor(string_accessor *);

		size_t count() const;
		void get(void *str, size_t *len, int &el_siz) const;
		void set(const void *str, size_t total_bytes);
	};
}

using namespace ds;

// ===================================================================================================
lookup::~lookup() {	
}

lookup *
lookup::create(const error_handler &ref, const string &key, type_t int_type, type_t ext_type) {
	lookup * result = NULL;
	
	switch(KEY(int_type, ext_type)) {
	case KEY(DS_T_STRING8, DS_T_UINT8):
		result = new lookup_impl<char, uint8_t>(key);
		break;
		
	case KEY(DS_T_STRING8, DS_T_UINT16):
		result = new lookup_impl<char, uint16_t>(key);
		break;
		
	case KEY(DS_T_STRING8, DS_T_UINT32):
		result = new lookup_impl<char, uint32_t>(key);
		break;
		
	case KEY(DS_T_STRING8, DS_T_UINT64):
		result = new lookup_impl<char, uint64_t>(key);
		break;
  // **********************		
	case KEY(DS_T_STRING16, DS_T_UINT8):
		result = new lookup_impl<short, uint8_t>(key);
		break;
		
	case KEY(DS_T_STRING16, DS_T_UINT16):
		result = new lookup_impl<short, uint16_t>(key);
		break;
		
	case KEY(DS_T_STRING16, DS_T_UINT32):
		result = new lookup_impl<short, uint32_t>(key);
		break;
		
	case KEY(DS_T_STRING16, DS_T_UINT64):
		result = new lookup_impl<short, uint64_t>(key);
		break;

	// ************************
	case KEY(DS_T_STRING32, DS_T_UINT8):
		result = new lookup_impl<int, uint8_t>(key);
		break;
		
	case KEY(DS_T_STRING32, DS_T_UINT16):
		result = new lookup_impl<int, uint16_t>(key);
		break;
		
	case KEY(DS_T_STRING32, DS_T_UINT32):
		result = new lookup_impl<int, uint32_t>(key);
		break;
		
	case KEY(DS_T_STRING32, DS_T_UINT64):
		result = new lookup_impl<int, uint64_t>(key);
		break;
	}	
	
	result ->err.set(ref.err.get());
	result ->warn.set(ref.warn.get());
	result ->info.set(ref.info.get());
	return result;
}

// ===================================================================================================
template<typename T>void
default_string_accessor<T>::get(size_t k, const void *p, string_container &s) {
	const T * const*v = reinterpret_cast<const T * const*>(p);
	s.siz = sizeof(T);
	s.len = str_length(static_cast<const T *>(v[k]));
	s.str = v[k];
	
}
template<typename T>void
default_string_accessor<T>::set(size_t k, const string_container &s, void *p ) {
	const T * *v = static_cast<const T * *>(p);
	v[k] = static_cast<const T*>(s.str);
}
	

// ===================================================================================================

template<typename T, typename N>
lookup_impl<T, N>::lookup_impl(const string &key) : key_(key) {	
	set_string_accessor(NULL);
}

template<typename T, typename N>void 
lookup_impl<T, N>::lookup_index(const void *str, void *val, size_t from, size_t num) {
	N *out = static_cast<N *>(val);

	for (size_t k = 0; k < num; ++ k) {			
		string_container entry;
		string_accessor_ ->get(from + k, str, entry);
		typename str_to_idx_t::iterator i =  str_to_idx_.find( entry );
			 
		if (i == str_to_idx_.end()) {
			size_t n = idx_to_str_.size();
			//info << "add " << (const char *)entry.str << endl;
			if (numeric_limits<N>::max() < n) {
				warn << "lookup::lookup_index: generated index out of range: " << n << " >= " << numeric_limits<N>::max()  << endl;
			}

			switch(entry.siz) {
				case sizeof(char): 
					entry.str = str_dup<T, char>(static_cast<const char *>(entry.str), entry.len);
					break;
					
				case sizeof(short): 
					entry.str = str_dup<T, short>(static_cast<const short *>(entry.str), entry.len);
					break;
					
				case sizeof(int): 
					entry.str = str_dup<T, int>(static_cast<const int *>(entry.str), entry.len);
					break;				
			}
			
			entry.siz = sizeof(T);
			
			idx_to_str_.push_back( entry );
			str_to_idx_.insert( make_pair( entry, n ) );
			out[k] = n;
		 } else {
			 out[k] = i ->second;
		 }
	}		
}

template<typename T, typename N>void 
lookup_impl<T, N>::lookup_value(const void * val, void *str, size_t from, size_t num) {
	const N *in = static_cast<const N *>(val);
	
	for (size_t k = 0; k < num; ++ k) {		
		string_accessor_ ->set(from + k, idx_to_str_[ in[k] ], str);
	}
}

template<typename T, typename N>size_t 
lookup_impl<T, N>::count() const {
	return idx_to_str_.size();
}

template<typename T, typename N>void 
lookup_impl<T, N>::get(void *str, size_t *len, int &el_siz) const {
	const void **arr = static_cast<const void **>(str);
	for ( size_t k = 0; k < count(); ++ k ) {
		arr[k] = idx_to_str_[k].str;
		len[k] = idx_to_str_[k].len;
	}
	
	el_siz = sizeof(T);
}

template<typename T, typename N>void 
lookup_impl<T, N>::set(const void *str, size_t total_bytes) {
	const T *t = static_cast<const T*>(str);
	while(total_bytes > 0) {
		size_t n = idx_to_str_.size();
		
		if (n >= numeric_limits<N>::max()) {
				warn << "lookup::set: generated index out of range: " << n << " >= " << numeric_limits<N>::max()  << endl;
		}

        string_container entry;
		entry.len  = str_length<T>(t);
		entry.siz = sizeof(T);
		entry.str = str_dup<T>(t, entry.len);
		
		idx_to_str_.push_back(entry);
		str_to_idx_.insert( make_pair( entry, n ) );

        t           += entry.len + 1;
        total_bytes -= (entry.len + 1) * sizeof(T);
	}
}


template<typename T, typename N>const string &
lookup_impl<T, N>::key() const {
	return key_;
}

template<typename T, typename N>void 
lookup_impl<T, N>::set_string_accessor(string_accessor *acc) {
	if (acc == NULL) {
		string_accessor_ = & default_string_accessor_;
	} else {
		string_accessor_ = acc;
	}
}
