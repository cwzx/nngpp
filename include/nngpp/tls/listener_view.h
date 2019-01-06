#ifndef NNGPP_TLS_LISTENER_VIEW_H
#define NNGPP_TLS_LISTENER_VIEW_H
#include <nng/supplemental/tls/tls.h>
#include <nngpp/aio_view.h>

namespace nng { namespace tls {

struct listener_view {
protected:
	nng_tls_listener* d = nullptr;
	
public:
	listener_view() = default;
	
	listener_view( nng_tls_listener* d ) noexcept : d(d) {}

	nng_tls_listener* get() const noexcept {
		return d;
	}

	nng_tls_listener* operator->() const noexcept {
		return d;
	}

	explicit operator bool() const noexcept {
		return d != nullptr;
	}
	
	void close() const noexcept {
		nng_tls_listener_close(d);
	}
	
	void listen( nng_sockaddr& addr ) const {
		int r = nng_tls_listener_listen(d,&addr);
		if( r != 0 ) {
			throw exception(r,"nng_tls_listener_listen");
		}
	}

	void accept( aio_view a ) const noexcept {
		nng_tls_listener_accept(d,a.get());
	}
	
	void get_opt( const char* name, void* val, size_t* szp ) const {
		int r = nng_tls_listener_getopt(d,name,val,szp);
		if( r != 0 ) {
			throw exception(r,"nng_tls_listener_getopt");
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
		int r = nng_tls_listener_setopt(d,name,val,sz);
		if( r != 0 ) {
			throw exception(r,"nng_tls_listener_setopt");
		}
	}
	
	template<typename T>
	void set_opt( const char* name, const T& v ) const {
		set_opt(name,&v,sizeof(v));
	}

};

inline nng_sockaddr get_opt_local_address( listener_view l ) {
	return l.get_opt<nng_sockaddr>( to_name(nng::option::local_address) );
}

inline bool get_opt_no_delay( listener_view l ) {
	return l.get_opt<bool>( to_name(option::no_delay) );
}

inline bool get_opt_keep_alive( listener_view l ) {
	return l.get_opt<bool>( to_name(option::keep_alive) );
}

inline void set_opt_no_delay( listener_view l, bool v ) {
	l.set_opt( to_name(option::no_delay), v );
}

inline void set_opt_keep_alive( listener_view l, bool v ) {
	l.set_opt( to_name(option::keep_alive), v );
}

}}

#endif
