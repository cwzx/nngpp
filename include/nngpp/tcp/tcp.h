#ifndef NNGPP_TCP_TCP_H
#define NNGPP_TCP_TCP_H
#include "tcp_view.h"
#include "dialer.h"
#include "listener.h"

namespace nng { namespace tcp {

struct tcp : tcp_view {

	tcp() = default;
	
	explicit tcp( nng_tcp* c ) noexcept : tcp_view(c) {}
	
	tcp( const tcp& rhs ) = delete;
	
	tcp( tcp&& rhs ) noexcept : tcp_view(rhs.c) {
		rhs.c = nullptr;
	}
	
	tcp& operator=( const tcp& rhs ) = delete;
	
	tcp& operator=( tcp&& rhs ) noexcept {
		if( this != &rhs ) {
			if( c != nullptr ) nng_tcp_free(c);
			c = rhs.c;
			rhs.c = nullptr;
		}
		return *this;
	}
	
	~tcp() {
		if( c != nullptr ) nng_tcp_free(c);
	}
	
	nng_tcp* release() noexcept {
		auto out = c;
		c = nullptr;
		return out;
	}

};

}}

#endif
