#ifndef NNGPP_IPC_LISTENER_VIEW_H
#define NNGPP_IPC_LISTENER_VIEW_H
#include <nng/supplemental/ipc/ipc.h>
#include <nngpp/aio_view.h>

namespace nng { namespace ipc {

struct listener_view {
protected:
	nng_ipc_listener* d = nullptr;
	
public:
	listener_view() = default;
	
	listener_view( nng_ipc_listener* d ) noexcept : d(d) {}

	nng_ipc_listener* get() const noexcept {
		return d;
	}

	nng_ipc_listener* operator->() const noexcept {
		return d;
	}

	explicit operator bool() const noexcept {
		return d != nullptr;
	}
	
	void close() const noexcept {
		nng_ipc_listener_close(d);
	}
	
	void listen( nng_sockaddr& addr ) const {
		int r = nng_ipc_listener_listen(d,&addr);
		if( r != 0 ) {
			throw exception(r,"nng_ipc_listener_listen");
		}
	}

	void accept( aio_view a ) const noexcept {
		nng_ipc_listener_accept(d,a.get());
	}
	
};

}}

#endif
