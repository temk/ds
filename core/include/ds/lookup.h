#ifndef __DS_LOOKUP_H__
#define __DS_LOOKUP_H__

#include <map>
#include <vector>
#include <iostream>
#include <stdexcept>
#include <string.h>

using namespace std;
namespace ds {

	class lookup {
	public:
		virtual ~lookup();
		
		virtual void lookup_index(const void *, void *, size_t num) = 0;
		virtual void lookup_value(const void *, void *, size_t num) = 0;
		
		virtual size_t size() const = 0;
		virtual void dump(void *buff, size_t buff_siz) const = 0;
		virtual void load(const void *buff, size_t buff_siz) = 0;
	};

	template<typename STR_T, typename INT_T> 
	class lookup_impl : public lookup {
	private:
	    typedef typename STR_T::value_type base_t;
		typedef vector<STR_T> idx_to_str_t;
		typedef map<STR_T, INT_T> str_to_idx_t;
		
		size_t size_; 
		str_to_idx_t str_to_idx_;
		idx_to_str_t idx_to_str_;
	public:
		lookup_impl();
		
		void lookup_index(const void *, void *, size_t num);
		void lookup_value(const void *, void *, size_t num);
		
		size_t size() const;
		void dump(void *buff, size_t buff_siz) const;
		void load(const void *buff, size_t buff_siz);
	};
	
	// ==============================================================================
	template<typename STR_T, typename INT_T> 
	lookup_impl<STR_T, INT_T>::lookup_impl() : size_(0) {
	}
	
	template<typename STR_T, typename INT_T> void 
	lookup_impl<STR_T, INT_T>::lookup_index(const void *src, void *dst, size_t num) {
		const base_t * const * in = static_cast<const base_t * const *>(src);
		INT_T *out = static_cast<INT_T *>(dst);
		
		for (size_t k = 0; k < num; ++ k) {
				typename str_to_idx_t::const_iterator i = str_to_idx_.find(in[k]);
				
				if (i == str_to_idx_.end()) {
					idx_to_str_.push_back(in[k]);
					size_ += idx_to_str_.back().length();
					str_to_idx_[in[k]] = idx_to_str_.size() - 1;
					out[k] = idx_to_str_.size() - 1;
				} else {
					out[k] = i ->second;
				}
		}
	}

	template<typename STR_T, typename INT_T> void 
	lookup_impl<STR_T, INT_T>::lookup_value(const void *src, void *dst, size_t num) {
		const INT_T *in = static_cast<const INT_T *>(src);
		const base_t ** out = static_cast< const base_t **>(dst);
		
		for (size_t k = 0; k < num; ++ k) {		
			if (in[k] > idx_to_str_.size()) {
				throw runtime_error("lookup::lookup_value: Index out of bound");
			}
			
			out[k] = idx_to_str_[in[k]].c_str();
		}
	}

	template<typename STR_T, typename INT_T> size_t
	lookup_impl<STR_T, INT_T>::size() const {
		return (size_ + idx_to_str_.size()) * sizeof(base_t);
	}
	
	template<typename STR_T, typename INT_T> void
	lookup_impl<STR_T, INT_T>::dump(void *buff, size_t buff_siz) const {
		base_t *data = static_cast<base_t *>(buff);
		
		for (typename idx_to_str_t::const_iterator iter = idx_to_str_.begin(); iter != idx_to_str_.end(); ++ iter) {
			memcpy(data, iter ->c_str(), (iter ->length() + 1) * sizeof(base_t)); 
			data += iter ->length() + 1;
		}
	}
		
	template<typename STR_T, typename INT_T> void
	lookup_impl<STR_T, INT_T>::load(const void *buff, size_t buff_siz) {
		const base_t *data = static_cast<const base_t *>(buff);
		while(buff_siz > 0) {
			idx_to_str_.push_back(data);
			size_t len = idx_to_str_.back().length() + 1;
			size_ += len - 1;
			data  += len;
			buff_siz -= len * sizeof(base_t);
		}
	}
	
	
}


#endif //__DS_LOOKUP_H__
