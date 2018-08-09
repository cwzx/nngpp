#ifndef NNGPP_MSG_HEADER_H
#define NNGPP_MSG_HEADER_H
#include "view.h"

namespace nng {

class msg_header {
	nng_msg* m;
	
public:
	
	explicit msg_header( nng_msg* m ) noexcept : m(m) {}
	
	nng_msg* get_msg() const noexcept {
		return m;
	}

	template<typename T = void>
	T* data() const noexcept {
		return (T*)nng_msg_header(m);
	}

	size_t size() const noexcept {
		return nng_msg_header_len(m);
	}

	view get() const noexcept {
		return view(
			nng_msg_header(m),
			nng_msg_header_len(m)
		);
	}

	void clear() const noexcept {
		nng_msg_header_clear(m);
	}
	
	void append( view v ) const {
		int r = nng_msg_header_append(m,v.data(),v.size());
		if( r != 0 ) {
			throw exception(r,"nng_msg_header_append");
		}
	}
	void insert( view v ) const {
		int r = nng_msg_header_insert(m,v.data(),v.size());
		if( r != 0 ) {
			throw exception(r,"nng_msg_header_insert");
		}
	}
	
	void trim( size_t size ) const {
		int r = nng_msg_header_trim(m,size);
		if( r != 0 ) {
			throw exception(r,"nng_msg_header_trim");
		}
	}
	
	void chop( size_t size ) const {
		int r = nng_msg_header_chop(m,size);
		if( r != 0 ) {
			throw exception(r,"nng_msg_header_chop");
		}
	}
	
	void append_u32( uint32_t val ) const {
		int r = nng_msg_header_append_u32(m,val);
		if( r != 0 ) {
			throw exception(r,"nng_msg_header_append_u32");
		}
	}
	
	void insert_u32( uint32_t val ) const {
		int r = nng_msg_header_insert_u32(m,val);
		if( r != 0 ) {
			throw exception(r,"nng_msg_header_insert_u32");
		}
	}
	
	uint32_t trim_u32() const {
		uint32_t val;
		int r = nng_msg_header_trim_u32(m,&val);
		if( r != 0 ) {
			throw exception(r,"nng_msg_header_trim_u32");
		}
		return val;
	}
	
	uint32_t chop_u32() const {
		uint32_t val;
		int r = nng_msg_header_chop_u32(m,&val);
		if( r != 0 ) {
			throw exception(r,"nng_msg_header_chop_u32");
		}
		return val;
	}

};

}

#endif
