#ifndef NNGPP_LISTENER_VIEW_H
#define NNGPP_LISTENER_VIEW_H
#include "option.h"
#include "buffer.h"

namespace nng {

struct listener_view {
protected:
	nng_listener lid{0};
	
public:
	listener_view() = default;
	
	listener_view( nng_listener lid ) noexcept : lid(lid) {}
	
	nng_listener get() const noexcept {
		return lid;
	}
	
	explicit operator bool() const noexcept {
		return lid.id != 0;
	}
	
	void start( int flags = 0 ) const {
		int r = nng_listener_start(lid,flags);
		if( r != 0 ) {
			throw exception(r,"nng_listener_start");
		}
	}
	
	int id() const noexcept {
		return nng_listener_id(lid);
	}
	
	void set_opt( const char* name, view v ) const {
		int r = nng_listener_setopt(lid,name,v.data(),v.size());
		if( r != 0 ) {
			throw exception(r,"nng_listener_setopt");
		}
	}
	
	void set_opt_bool( const char* name, bool value ) const {
		int r = nng_listener_setopt_bool(lid,name,value);
		if( r != 0 ) {
			throw exception(r,"nng_listener_setopt_bool");
		}
	}
	
	void set_opt_int( const char* name, int value ) const {
		int r = nng_listener_setopt_int(lid,name,value);
		if( r != 0 ) {
			throw exception(r,"nng_listener_setopt_int");
		}
	}
	
	void set_opt_ms( const char* name, nng_duration value ) const {
		int r = nng_listener_setopt_ms(lid,name,value);
		if( r != 0 ) {
			throw exception(r,"nng_listener_setopt_ms");
		}
	}
	
	void set_opt_size( const char* name, size_t value ) const {
		int r = nng_listener_setopt_size(lid,name,value);
		if( r != 0 ) {
			throw exception(r,"nng_listener_setopt_size");
		}
	}
	
	void set_opt_uint64( const char* name, uint64_t value ) const {
		int r = nng_listener_setopt_uint64(lid,name,value);
		if( r != 0 ) {
			throw exception(r,"nng_listener_setopt_uint64");
		}
	}
	
	void set_opt_string( const char* name, const char* value ) const {
		int r = nng_listener_setopt_string(lid,name,value);
		if( r != 0 ) {
			throw exception(r,"nng_listener_setopt_string");
		}
	}
	
	void set_opt_ptr( const char* name, void* value ) const {
		int r = nng_listener_setopt_ptr(lid,name,value);
		if( r != 0 ) {
			throw exception(r,"nng_listener_setopt_ptr");
		}
	}
	
	size_t get_opt( const char* name, buffer& out ) const {
		size_t size = out.size();
		int r = nng_listener_getopt(lid,name,out.data(),&size);
		if( r != 0 ) {
			throw exception(r,"nng_listener_getopt");
		}
		return size;
	}

	bool get_opt_bool( const char* name ) const {
		bool out;
		int r = nng_listener_getopt_bool(lid,name,&out);
		if( r != 0 ) {
			throw exception(r,"nng_listener_getopt_bool");
		}
		return out;
	}
	
	int get_opt_int( const char* name ) const {
		int out;
		int r = nng_listener_getopt_int(lid,name,&out);
		if( r != 0 ) {
			throw exception(r,"nng_listener_getopt_int");
		}
		return out;
	}
	
	nng_duration get_opt_ms( const char* name ) const {
		nng_duration out;
		int r = nng_listener_getopt_ms(lid,name,&out);
		if( r != 0 ) {
			throw exception(r,"nng_listener_getopt_ms");
		}
		return out;
	}
	
	size_t get_opt_size( const char* name ) const {
		size_t out;
		int r = nng_listener_getopt_size(lid,name,&out);
		if( r != 0 ) {
			throw exception(r,"nng_listener_getopt_size");
		}
		return out;
	}
	
	nng_sockaddr get_opt_sockaddr( const char* name ) const {
		nng_sockaddr out;
		int r = nng_listener_getopt_sockaddr(lid,name,&out);
		if( r != 0 ) {
			throw exception(r,"nng_listener_getopt_sockaddr");
		}
		return out;
	}

	uint64_t get_opt_uint64( const char* name ) const {
		uint64_t out;
		int r = nng_listener_getopt_uint64(lid,name,&out);
		if( r != 0 ) {
			throw exception(r,"nng_listener_getopt_uint64");
		}
		return out;
	}
	
	void* get_opt_ptr( const char* name ) const {
		void* out;
		int r = nng_listener_getopt_ptr(lid,name,&out);
		if( r != 0 ) {
			throw exception(r,"nng_listener_getopt_ptr");
		}
		return out;
	}
	
	buffer get_opt_string( const char* name ) const {
		char* data;
		int r = nng_listener_getopt_string(lid,name,&data);
		if( r != 0 ) {
			throw exception(r,"nng_listener_getopt_string");
		}
		return buffer(data,strlen(data)+1);
	}
};

inline nng_sockaddr get_opt_local_address( listener_view s ) {
	return s.get_opt_sockaddr( to_name(option::local_address) );
}

inline buffer get_opt_url( listener_view s ) {
	return s.get_opt_string( to_name(option::url) );
}

inline size_t get_opt_recv_size_max( listener_view s ) {
	return s.get_opt_size( to_name(option::recv_size_max) );
}

inline void set_opt_recv_size_max( listener_view s, size_t v ) {
	s.set_opt_size( to_name(option::recv_size_max), v );
}

}

#endif
