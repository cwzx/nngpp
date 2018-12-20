#ifndef NNGPP_TCP_LISTENER_H
#define NNGPP_TCP_LISTENER_H
#include "listener_view.h"

namespace nng { namespace tcp {

struct listener : listener_view {

	listener() = default;
	
	explicit listener( nng_tcp_listener* d ) noexcept : listener_view(d) {}
	
	listener( const listener& rhs ) = delete;
	
	listener( listener&& rhs ) noexcept : listener_view(rhs.d) {
		rhs.d = nullptr;
	}
	
	listener& operator=( const listener& rhs ) = delete;
	
	listener& operator=( listener&& rhs ) noexcept {
		if( this != &rhs ) {
			if( d != nullptr ) nng_tcp_listener_free(d);
			d = rhs.d;
			rhs.d = nullptr;
		}
		return *this;
	}
	
	~listener() {
		if( d != nullptr ) nng_tcp_listener_free(d);
	}
	
	nng_tcp_listener* release() noexcept {
		auto out = d;
		d = nullptr;
		return out;
	}

};

inline listener make_listener() {
	nng_tcp_listener* d;
	int r = nng_tcp_listener_alloc( &d );
	if( r != 0 ) {
		throw exception(r,"nng_tcp_listener_alloc");
	}
	return listener(d);
}

}}

#endif
