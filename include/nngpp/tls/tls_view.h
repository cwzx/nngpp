#ifndef NNGPP_TLS_TLS_VIEW_H
#define NNGPP_TLS_TLS_VIEW_H
#include <nng/supplemental/tls/tls.h>
#include <nngpp/transport/tls.h>
#include <nngpp/transport/tcp.h>
#include <nngpp/aio_view.h>

namespace nng { namespace tls {

struct tls_view {
protected:
	nng_tls* c = nullptr;
	
public:
	tls_view() = default;
	
	tls_view( nng_tls* c ) noexcept : c(c) {}

	nng_tls* get() const noexcept {
		return c;
	}

	nng_tls* operator->() const noexcept {
		return c;
	}

	explicit operator bool() const noexcept {
		return c != nullptr;
	}

	void close() const noexcept {
		nng_tls_close(c);
	}
	
	void send( aio_view a ) const noexcept {
		nng_tls_send(c,a.get());
	}
	
	void recv( aio_view a ) const noexcept {
		nng_tls_recv(c,a.get());
	}

	void get_opt( const char* name, void* val, size_t* szp ) const {
		int r = nng_tls_getopt(c,name,val,szp);
		if( r != 0 ) {
			throw exception(r,"nng_tls_getopt");
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
		int r = nng_tls_setopt(c,name,val,sz);
		if( r != 0 ) {
			throw exception(r,"nng_tls_setopt");
		}
	}
	
	template<typename T>
	void set_opt( const char* name, const T& v ) const {
		set_opt(name,&v,sizeof(v));
	}

};

inline nng_sockaddr get_opt_local_address( tls_view c ) {
	return c.get_opt<nng_sockaddr>( to_name(nng::option::local_address) );
}

inline nng_sockaddr get_opt_remote_address( tls_view c ) {
	return c.get_opt<nng_sockaddr>( to_name(nng::option::remote_address) );
}

inline bool get_opt_no_delay( tls_view c ) {
	return c.get_opt<bool>( to_name(tcp::option::no_delay) );
}

inline bool get_opt_keep_alive( tls_view c ) {
	return c.get_opt<bool>( to_name(tcp::option::keep_alive) );
}

inline bool get_opt_verified( tls_view c ) {
	return c.get_opt<bool>( to_name(option::verified) );
}

inline void set_opt_no_delay( tls_view c, bool v ) {
	c.set_opt( to_name(tcp::option::no_delay), v );
}

inline void set_opt_keep_alive( tls_view c, bool v ) {
	c.set_opt( to_name(tcp::option::keep_alive), v );
}

}}

#endif
