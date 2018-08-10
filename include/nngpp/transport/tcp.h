#ifndef NNGPP_TRANSPORT_TCP_H
#define NNGPP_TRANSPORT_TCP_H
#include <nngpp/listener.h>
#include <nngpp/dialer.h>
#include <nngpp/pipe.h>

extern "C" {
#include <nng/transport/tcp/tcp.h>
}

namespace nng { namespace tcp {

inline void register_transport() {
	int r = nng_tcp_register();
	if( r != 0 ) {
		throw exception(r,"nng_tcp_register");
	}
}

enum class option {
	no_delay,
	keep_alive
};

namespace detail {

#if __cpp_inline_variables >= 201606
inline
#else
static
#endif
const char* const option_names[] = {
	NNG_OPT_TCP_NODELAY,  // bool
	NNG_OPT_TCP_KEEPALIVE // bool
};

}

inline const char* to_name( option o ) noexcept {
	return detail::option_names[(int)o];
}

inline bool get_opt_no_delay( pipe_view s ) {
	return s.get_opt_bool( to_name(option::no_delay) );
}

inline bool get_opt_keep_alive( pipe_view s ) {
	return s.get_opt_bool( to_name(option::keep_alive) );
}

inline bool get_opt_no_delay( dialer_view s ) {
	return s.get_opt_bool( to_name(option::no_delay) );
}

inline bool get_opt_keep_alive( dialer_view s ) {
	return s.get_opt_bool( to_name(option::keep_alive) );
}

inline bool get_opt_no_delay( listener_view s ) {
	return s.get_opt_bool( to_name(option::no_delay) );
}

inline bool get_opt_keep_alive( listener_view s ) {
	return s.get_opt_bool( to_name(option::keep_alive) );
}

inline bool get_opt_no_delay( socket_view s ) {
	return s.get_opt_bool( to_name(option::no_delay) );
}

inline bool get_opt_keep_alive( socket_view s ) {
	return s.get_opt_bool( to_name(option::keep_alive) );
}

inline void set_opt_no_delay( dialer_view s, bool v ) {
	s.set_opt_bool( to_name(option::no_delay), v );
}

inline void set_opt_keep_alive( dialer_view s, bool v ) {
	s.set_opt_bool( to_name(option::keep_alive), v );
}

inline void set_opt_no_delay( listener_view s, bool v ) {
	s.set_opt_bool( to_name(option::no_delay), v );
}

inline void set_opt_keep_alive( listener_view s, bool v ) {
	s.set_opt_bool( to_name(option::keep_alive), v );
}

inline void set_opt_no_delay( socket_view s, bool v ) {
	s.set_opt_bool( to_name(option::no_delay), v );
}

inline void set_opt_keep_alive( socket_view s, bool v ) {
	s.set_opt_bool( to_name(option::keep_alive), v );
}

}}

#endif
