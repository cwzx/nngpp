#ifndef NNGPP_LISTENER_H
#define NNGPP_LISTENER_H
#include "listener_view.h"
#include "socket_view.h"

namespace nng {

struct listener : listener_view {
	
	explicit listener( nng_listener lid ) noexcept : listener_view(lid) {}
	
	explicit listener( socket_view s, const char* addr, int flags ) {
		int r = nng_listen( s.get(), addr, &lid, flags );
		if( r != 0 ) {
			throw exception(r,"nng_listen");
		}
	}
	
	explicit listener( socket_view s, const char* addr ) {
		int r = nng_listener_create( &lid, s.get(), addr );
		if( r != 0 ) {
			throw exception(r,"nng_listener_create");
		}
	}
	
	listener( const listener& rhs ) = delete;
	
	listener( listener&& rhs ) noexcept : listener_view(rhs.lid) {
		rhs.lid.id = 0;
	}
	
	listener& operator=( const listener& rhs ) = delete;
	
	listener& operator=( listener&& rhs ) {
		if( this != &rhs ) {
			if( lid.id != 0 ) {
				int r = nng_listener_close(lid);
				if( r != 0 && r != (int)nng::error::closed ) {
					throw exception(r,"nng_listener_close");
				}
			}
			lid = rhs.lid;
			rhs.lid.id = 0;
		}
		return *this;
	}
	
	~listener() {
		if( lid.id != 0 ) nng_listener_close(lid);
	}

	nng_listener release() noexcept {
		auto out = lid;
		lid.id = 0;
		return out;
	}

};

}

#endif
