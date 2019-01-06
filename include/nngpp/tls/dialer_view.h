#ifndef NNGPP_TLS_DIALER_VIEW_H
#define NNGPP_TLS_DIALER_VIEW_H
#include <nngpp/aio_view.h>
#include <nng/supplemental/tls/tls.h>

namespace nng { namespace tls {

struct dialer_view {
protected:
	nng_tls_dialer* d = nullptr;
	
public:
	dialer_view() = default;
	
	dialer_view( nng_tls_dialer* d ) noexcept : d(d) {}

	nng_tls_dialer* get() const noexcept {
		return d;
	}

	nng_tls_dialer* operator->() const noexcept {
		return d;
	}

	explicit operator bool() const noexcept {
		return d != nullptr;
	}
	
	void close() const noexcept {
		nng_tls_dialer_close(d);
	}
	
	void dial( const nng_sockaddr& addr, aio_view a ) const noexcept {
		nng_tls_dialer_dial(d,&addr,a.get());
	}
	
	void get_opt( const char* name, void* val, size_t* szp ) const {
		int r = nng_tls_dialer_getopt(d,name,val,szp);
		if( r != 0 ) {
			throw exception(r,"nng_tls_dialer_getopt");
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
		int r = nng_tls_dialer_setopt(d,name,val,sz);
		if( r != 0 ) {
			throw exception(r,"nng_tls_dialer_setopt");
		}
	}
	
	template<typename T>
	void set_opt( const char* name, const T& v ) const {
		set_opt(name,&v,sizeof(v));
	}
	
};

inline nng_sockaddr get_opt_local_address( dialer_view d ) {
	return d.get_opt<nng_sockaddr>( to_name(nng::option::local_address) );
}

inline bool get_opt_no_delay( dialer_view d ) {
	return d.get_opt<bool>( to_name(tcp::option::no_delay) );
}

inline bool get_opt_keep_alive( dialer_view d ) {
	return d.get_opt<bool>( to_name(tcp::option::keep_alive) );
}

inline void set_opt_local_address( dialer_view d, const nng_sockaddr& v ) {
	return d.set_opt( to_name(nng::option::local_address), v );
}

inline void set_opt_no_delay( dialer_view d, bool v ) {
	d.set_opt( to_name(tcp::option::no_delay), v );
}

inline void set_opt_keep_alive( dialer_view d, bool v ) {
	d.set_opt( to_name(tcp::option::keep_alive), v );
}

}}

#endif
