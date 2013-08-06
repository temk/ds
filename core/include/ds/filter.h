#ifndef __DS_FILTER_H__
#define __DS_FILTER_H__
#include <ds/types.h>

namespace ds {
	
	class filter {
	private:
		size_t s_in_;
		size_t s_out_;
		
		type_t t_in_;
		type_t t_out_;
		
		filter *next_;
		
	public:
		filter(type_t t_in, size_t s_in, type_t t_out, size_t s_out);
		virtual ~filter();		
		
		virtual void flush();
		
		virtual void put(const void *data, size_t num);
		virtual void get(size_t offs, size_t num, void *data);
		virtual void get(const void *indexes, int idx_siz, size_t num, void *data);

		inline type_t type_in()  const;
		inline type_t type_out() const;
				
		inline size_t size_in()  const;
		inline size_t size_out() const;
				
		inline const filter *next() const;		
		inline filter *next();
		void next(filter *);
	};
	
	// ===============================================================
	inline const filter *filter::next() const {
		return next_;
	}
	
	inline filter *filter::next() {
		return next_;
	}
	

	inline type_t filter::type_in() const {
		return t_in_;
	}
	
	inline type_t filter::type_out() const {
		return t_out_;
	}

	inline size_t filter::size_in() const {
		return s_in_;
	}
	
	inline size_t filter::size_out() const {
		return s_out_;
	}
}

#endif // __DS_FILTER_H__
