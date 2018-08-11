#ifndef NNGPP_SNAPSHOT_H
#define NNGPP_SNAPSHOT_H
#include "snapshot_view.h"
#include "socket_view.h"

namespace nng {

struct snapshot : snapshot_view {

	snapshot() = default;

	explicit snapshot( nng_snapshot* s ) noexcept : snapshot_view(s) {}

	explicit snapshot( socket_view sock ) {
		int r = nng_snapshot_create( sock.get(), &s );
		if( r != 0 ) {
			throw exception(r,"nng_snapshot_create");
		}
	}

	snapshot( const snapshot& rhs ) = delete;
	
	snapshot( snapshot&& rhs ) noexcept : snapshot_view(rhs.s) {
		rhs.s = nullptr;
	}
	
	snapshot& operator=( const snapshot& rhs ) = delete;
	
	snapshot& operator=( snapshot&& rhs ) noexcept {
		if( this != &rhs ) {
			if( s != nullptr ) nng_snapshot_free(s);
			s = rhs.s;
			rhs.s = nullptr;
		}
		return *this;
	}
	
	~snapshot() {
		if( s != nullptr ) nng_snapshot_free(s);
	}

	nng_snapshot* release() {
		auto out = s;
		s = nullptr;
		return out;
	}

};

inline snapshot make_snapshot( socket_view sock ) {
	return snapshot(sock);
}

}

#endif
