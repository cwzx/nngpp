#ifndef NNGPP_TCP_TCP_VIEW_H
#define NNGPP_TCP_TCP_VIEW_H
#include <nng/supplemental/tcp/tcp.h>
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
	
	nng_sockaddr sockname() const {
		nng_sockaddr out;
		int r = nng_tcp_sockname(c,&out);
		if( r != 0 ) {
			throw exception(r,"nng_tcp_sockname");
		}
		return out;
	}
	
	nng_sockaddr peername() const {
		nng_sockaddr out;
		int r = nng_tcp_peername(c,&out);
		if( r != 0 ) {
			throw exception(r,"nng_tcp_peername");
		}
		return out;
	}
	
	void set_nodelay( bool val ) const {
		int r = nng_tcp_set_nodelay(c,val);
		if( r != 0 ) {
			throw exception(r,"nng_tcp_set_nodelay");
		}
	}
	
	void set_keepalive( bool val ) const {
		int r = nng_tcp_set_keepalive(c,val);
		if( r != 0 ) {
			throw exception(r,"nng_tcp_set_keepalive");
		}
	}
	
};

inline nng_sockaddr get_opt_socket_name( tcp_view c ) {
	return c.sockname();
}

inline nng_sockaddr get_opt_peer_name( tcp_view c ) {
	return c.peername();
}

inline void set_opt_no_delay( tcp_view s, bool v ) {
	s.set_nodelay(v);
}

inline void set_opt_keep_alive( tcp_view s, bool v ) {
	s.set_keepalive(v);
}

}}

#endif
