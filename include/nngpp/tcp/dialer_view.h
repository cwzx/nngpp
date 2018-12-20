#ifndef NNGPP_TCP_DIALER_VIEW_H
#define NNGPP_TCP_DIALER_VIEW_H
#include <nng/supplemental/tcp/tcp.h>
#include <nngpp/aio_view.h>

namespace nng { namespace tcp {

struct dialer_view {
protected:
	nng_tcp_dialer* d = nullptr;
	
public:
	dialer_view() = default;
	
	dialer_view( nng_tcp_dialer* d ) noexcept : d(d) {}

	nng_tcp_dialer* get() const noexcept {
		return d;
	}

	nng_tcp_dialer* operator->() const noexcept {
		return d;
	}

	explicit operator bool() const noexcept {
		return d != nullptr;
	}
	
	void close() const noexcept {
		nng_tcp_dialer_close(d);
	}
	
	void set_source( const nng_sockaddr& addr ) const {
		int r = nng_tcp_dialer_set_source(d,&addr);
		if( r != 0 ) {
			throw exception(r,"nng_tcp_dialer_set_source");
		}
	}
	
	void dial( const nng_sockaddr& addr, aio_view a ) const noexcept {
		nng_tcp_dialer_dial(d,&addr,a.get());
	}
	
};

}}

#endif
