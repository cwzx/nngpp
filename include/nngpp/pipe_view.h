#ifndef NNGPP_PIPE_VIEW_H
#define NNGPP_PIPE_VIEW_H
#include "dialer_view.h"
#include "listener_view.h"

namespace nng {

struct socket_view;

struct pipe_view {
protected:
	nng_pipe p{0};
	
public:
	pipe_view() = default;
	
	pipe_view( nng_pipe p ) noexcept : p(p) {}
	
	nng_pipe get() const noexcept {
		return p;
	}

	explicit operator bool() const noexcept {
		return p.id != 0;
	}
	
	int id() const noexcept {
		return nng_pipe_id(p);
	}
	
	size_t get_opt( const char* name, buffer& out ) const {
		size_t size = out.size();
		int r = nng_pipe_getopt(p,name,out.data(),&size);
		if( r != 0 ) {
			throw exception(r,"nng_pipe_getopt");
		}
		return size;
	}

	bool get_opt_bool( const char* name ) const {
		bool out;
		int r = nng_pipe_getopt_bool(p,name,&out);
		if( r != 0 ) {
			throw exception(r,"nng_pipe_getopt_bool");
		}
		return out;
	}
	
	int get_opt_int( const char* name ) const {
		int out;
		int r = nng_pipe_getopt_int(p,name,&out);
		if( r != 0 ) {
			throw exception(r,"nng_pipe_getopt_int");
		}
		return out;
	}
	
	nng_duration get_opt_ms( const char* name ) const {
		nng_duration out;
		int r = nng_pipe_getopt_ms(p,name,&out);
		if( r != 0 ) {
			throw exception(r,"nng_pipe_getopt_ms");
		}
		return out;
	}
	
	size_t get_opt_size( const char* name ) const {
		size_t out;
		int r = nng_pipe_getopt_size(p,name,&out);
		if( r != 0 ) {
			throw exception(r,"nng_pipe_getopt_size");
		}
		return out;
	}

	nng_sockaddr get_opt_sockaddr( const char* name ) const {
		nng_sockaddr out;
		int r = nng_pipe_getopt_sockaddr(p,name,&out);
		if( r != 0 ) {
			throw exception(r,"nng_pipe_getopt_sockaddr");
		}
		return out;
	}

	uint64_t get_opt_uint64( const char* name ) const {
		uint64_t out;
		int r = nng_pipe_getopt_uint64(p,name,&out);
		if( r != 0 ) {
			throw exception(r,"nng_pipe_getopt_uint64");
		}
		return out;
	}
	
	void* get_opt_ptr( const char* name ) const {
		void* out;
		int r = nng_pipe_getopt_ptr(p,name,&out);
		if( r != 0 ) {
			throw exception(r,"nng_pipe_getopt_ptr");
		}
		return out;
	}
	
	buffer get_opt_string( const char* name ) const {
		char* data;
		int r = nng_pipe_getopt_string(p,name,&data);
		if( r != 0 ) {
			throw exception(r,"nng_pipe_getopt_string");
		}
		return buffer(data,strlen(data)+1);
	}

	socket_view get_socket() const noexcept;

	dialer_view get_dialer() const noexcept {
		return nng_pipe_dialer(p);
	}
	
	listener_view get_listener() const noexcept {
		return nng_pipe_listener(p);
	}
	
};

inline nng_sockaddr get_opt_local_address( pipe_view s ) {
	return s.get_opt_sockaddr( to_name(option::local_address) );
}

inline nng_sockaddr get_opt_remote_address( pipe_view s ) {
	return s.get_opt_sockaddr( to_name(option::remote_address) );
}

}

#endif
