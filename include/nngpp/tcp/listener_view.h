#ifndef NNGPP_TCP_LISTENER_VIEW_H
#define NNGPP_TCP_LISTENER_VIEW_H
#include <nng/supplemental/tcp/tcp.h>
#include <nngpp/aio_view.h>

namespace nng { namespace tcp {

struct listener_view {
protected:
	nng_tcp_listener* d = nullptr;
	
public:
	listener_view() = default;
	
	listener_view( nng_tcp_listener* d ) noexcept : d(d) {}

	nng_tcp_listener* get() const noexcept {
		return d;
	}

	nng_tcp_listener* operator->() const noexcept {
		return d;
	}

	explicit operator bool() const noexcept {
		return d != nullptr;
	}
	
	void close() const noexcept {
		nng_tcp_listener_close(d);
	}
	
	void listen( nng_sockaddr& addr ) const {
		int r = nng_tcp_listener_listen(d,&addr);
		if( r != 0 ) {
			throw exception(r,"nng_tcp_listener_listen");
		}
	}

	void accept( aio_view a ) const noexcept {
		nng_tcp_listener_accept(d,a.get());
	}
	
};

}}

#endif
