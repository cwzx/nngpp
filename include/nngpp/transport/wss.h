#ifndef NNGPP_TRANSPORT_WSS_H
#define NNGPP_TRANSPORT_WSS_H
#include <nngpp/error.h>

extern "C" {
#include <nng/transport/ws/websocket.h>
}

namespace nng::wss {

inline void register_transport() {
	int r = nng_wss_register();
	if( r != 0 ) {
		throw exception(r,"nng_wss_register");
	}
}

}

#endif
