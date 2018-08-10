#ifndef NNGPP_TRANSPORT_WS_H
#define NNGPP_TRANSPORT_WS_H
#include <nngpp/listener.h>
#include <nngpp/dialer.h>
#include <nngpp/pipe.h>

extern "C" {
#include <nng/transport/ws/websocket.h>
}

namespace nng::ws {

inline void register_transport() {
	int r = nng_ws_register();
	if( r != 0 ) {
		throw exception(r,"nng_ws_register");
	}
}

enum class option {
	request_headers, // string
	response_headers // string
};

namespace detail {

inline const char* const option_names[] = {
	NNG_OPT_WS_REQUEST_HEADERS,
	NNG_OPT_WS_RESPONSE_HEADERS
};

}

inline const char* to_name( option o ) noexcept {
	return detail::option_names[(int)o];
}

inline buffer get_opt_request_headers( pipe_view s ) {
	return s.get_opt_string( to_name(option::request_headers) );
}

inline buffer get_opt_response_headers( pipe_view s ) {
	return s.get_opt_string( to_name(option::response_headers) );
}

inline void set_opt_request_headers( dialer_view s, const char* v ) {
	s.set_opt_string( to_name(option::request_headers), v );
}

inline void set_opt_response_headers( listener_view s, const char* v ) {
	s.set_opt_string( to_name(option::response_headers), v );
}

}

#endif
