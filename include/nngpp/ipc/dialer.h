#ifndef NNGPP_IPC_DIALER_H
#define NNGPP_IPC_DIALER_H
#include "dialer_view.h"

namespace nng { namespace ipc {

struct dialer : dialer_view {

	dialer() = default;
	
	explicit dialer( nng_ipc_dialer* d ) noexcept : dialer_view(d) {}
	
	dialer( const dialer& rhs ) = delete;
	
	dialer( dialer&& rhs ) noexcept : dialer_view(rhs.d) {
		rhs.d = nullptr;
	}
	
	dialer& operator=( const dialer& rhs ) = delete;
	
	dialer& operator=( dialer&& rhs ) noexcept {
		if( this != &rhs ) {
			if( d != nullptr ) nng_ipc_dialer_free(d);
			d = rhs.d;
			rhs.d = nullptr;
		}
		return *this;
	}
	
	~dialer() {
		if( d != nullptr ) nng_ipc_dialer_free(d);
	}
	
	nng_ipc_dialer* release() noexcept {
		auto out = d;
		d = nullptr;
		return out;
	}

};

inline dialer make_dialer() {
	nng_ipc_dialer* d;
	int r = nng_ipc_dialer_alloc( &d );
	if( r != 0 ) {
		throw exception(r,"nng_ipc_dialer_alloc");
	}
	return dialer(d);
}

}}

#endif
