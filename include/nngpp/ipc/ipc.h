#ifndef NNGPP_IPC_IPC_H
#define NNGPP_IPC_IPC_H
#include "ipc_view.h"
#include "dialer.h"
#include "listener.h"

namespace nng { namespace ipc {

struct ipc : ipc_view {

	ipc() = default;
	
	explicit ipc( nng_ipc* c ) noexcept : ipc_view(c) {}
	
	ipc( const ipc& rhs ) = delete;
	
	ipc( ipc&& rhs ) noexcept : ipc_view(rhs.c) {
		rhs.c = nullptr;
	}
	
	ipc& operator=( const ipc& rhs ) = delete;
	
	ipc& operator=( ipc&& rhs ) noexcept {
		if( this != &rhs ) {
			if( c != nullptr ) nng_ipc_free(c);
			c = rhs.c;
			rhs.c = nullptr;
		}
		return *this;
	}
	
	~ipc() {
		if( c != nullptr ) nng_ipc_free(c);
	}
	
	nng_ipc* release() noexcept {
		auto out = c;
		c = nullptr;
		return out;
	}

};

}}

#endif
