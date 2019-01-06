#ifndef NNGPP_TLS_TLS_H
#define NNGPP_TLS_TLS_H
#include "tls_view.h"
#include "dialer.h"
#include "listener.h"

namespace nng { namespace tls {

struct tls : tls_view {

	tls() = default;
	
	explicit tls( nng_tls* c ) noexcept : tls_view(c) {}
	
	tls( const tls& rhs ) = delete;
	
	tls( tls&& rhs ) noexcept : tls_view(rhs.c) {
		rhs.c = nullptr;
	}
	
	tls& operator=( const tls& rhs ) = delete;
	
	tls& operator=( tls&& rhs ) noexcept {
		if( this != &rhs ) {
			if( c != nullptr ) nng_tls_free(c);
			c = rhs.c;
			rhs.c = nullptr;
		}
		return *this;
	}
	
	~tls() {
		if( c != nullptr ) nng_tls_free(c);
	}
	
	nng_tls* release() noexcept {
		auto out = c;
		c = nullptr;
		return out;
	}

};

}}

#endif
