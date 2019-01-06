#ifndef NNGPP_TCP_TCP_VIEW_H
#define NNGPP_TCP_TCP_VIEW_H
#include <nng/supplemental/tcp/tcp.h>
#include <nngpp/transport/tcp.h>
#include <nngpp/aio_view.h>

namespace nng { namespace tcp {

struct tcp_view {
protected:
	nng_tcp* c = nullptr;
	
public:
	tcp_view() = default;
	
	tcp_view( nng_tcp* c ) noexcept : c(c) {}

	nng_tcp* get() const noexcept {
		return c;
	}

	nng_tcp* operator->() const noexcept {
		return c;
	}

	explicit operator bool() const noexcept {
		return c != nullptr;
	}

	void close() const noexcept {
		nng_tcp_close(c);
	}
	
	void send( aio_view a ) const noexcept {
		nng_tcp_send(c,a.get());
	}
	
	void recv( aio_view a ) const noexcept {
		nng_tcp_recv(c,a.get());
	}

	void get_opt( const char* name, void* val, size_t* szp ) const {
		int r = nng_tcp_getopt(c,name,val,szp);
		if( r != 0 ) {
			throw exception(r,"nng_tcp_getopt");
		}
	}
	
	template<typename T>
	T get_opt( const char* name ) const {
		T out;
		size_t size = sizeof(out);
		get_opt(name,&out,&size);
		return out;
	}
	
	void set_opt( const char* name, const void* val, size_t sz ) const {
		int r = nng_tcp_setopt(c,name,val,sz);
		if( r != 0 ) {
			throw exception(r,"nng_tcp_setopt");
		}
	}
	
	template<typename T>
	void set_opt( const char* name, const T& v ) const {
		set_opt(name,&v,sizeof(v));
	}

};

inline nng_sockaddr get_opt_local_address( tcp_view c ) {
	return c.get_opt<nng_sockaddr>( to_name(nng::option::local_address) );
}

inline nng_sockaddr get_opt_remote_address( tcp_view c ) {
	return c.get_opt<nng_sockaddr>( to_name(nng::option::remote_address) );
}

inline bool get_opt_no_delay( tcp_view c ) {
	return c.get_opt<bool>( to_name(option::no_delay) );
}

inline bool get_opt_keep_alive( tcp_view c ) {
	return c.get_opt<bool>( to_name(option::keep_alive) );
}

inline void set_opt_no_delay( tcp_view c, bool v ) {
	c.set_opt( to_name(option::no_delay), v );
}

inline void set_opt_keep_alive( tcp_view c, bool v ) {
	c.set_opt( to_name(option::keep_alive), v );
}

}}

#endif
