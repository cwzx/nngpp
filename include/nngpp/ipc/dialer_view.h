#ifndef NNGPP_IPC_DIALER_VIEW_H
#define NNGPP_IPC_DIALER_VIEW_H
#include <nngpp/aio_view.h>
#include <nng/supplemental/ipc/ipc.h>

namespace nng { namespace ipc {

struct dialer_view {
protected:
	nng_ipc_dialer* d = nullptr;
	
public:
	dialer_view() = default;
	
	dialer_view( nng_ipc_dialer* d ) noexcept : d(d) {}

	nng_ipc_dialer* get() const noexcept {
		return d;
	}

	nng_ipc_dialer* operator->() const noexcept {
		return d;
	}

	explicit operator bool() const noexcept {
		return d != nullptr;
	}
	
	void close() const noexcept {
		nng_ipc_dialer_close(d);
	}

	void dial( const nng_sockaddr& addr, aio_view a ) const noexcept {
		nng_ipc_dialer_dial(d,&addr,a.get());
	}
	
};

}}

#endif
