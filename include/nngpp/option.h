#ifndef NNGPP_OPTION_H
#define NNGPP_OPTION_H
#include <nng/nng.h>

namespace nng {

enum class option {
	socket_name,        // string
	raw,                // bool
	protocol,           // int
	protocol_name,      // string
	peer,               // int
	peer_name,          // string
	recv_buffer,        // int
	send_buffer,        // int
	recv_fd,            // int
	send_fd,            // int
	recv_timeout,       // ms
	send_timeout,       // ms
	local_address,      // sockaddr
	remote_address,     // sockaddr
	url,                // string
	ttl_max,            // int
	recv_size_max,      // size
	reconnect_time_min, // ms
	reconnect_time_max  // ms
};

namespace detail {

#if __cpp_inline_variables >= 201606
inline
#else
static
#endif
const char* const option_names[] = {
	NNG_OPT_SOCKNAME,
	NNG_OPT_RAW,
	NNG_OPT_PROTO,
	NNG_OPT_PROTONAME,
	NNG_OPT_PEER,
	NNG_OPT_PEERNAME,
	NNG_OPT_RECVBUF,
	NNG_OPT_SENDBUF,
	NNG_OPT_RECVFD,
	NNG_OPT_SENDFD,
	NNG_OPT_RECVTIMEO,
	NNG_OPT_SENDTIMEO,
	NNG_OPT_LOCADDR,
	NNG_OPT_REMADDR,
	NNG_OPT_URL,
	NNG_OPT_MAXTTL,
	NNG_OPT_RECVMAXSZ,
	NNG_OPT_RECONNMINT,
	NNG_OPT_RECONNMAXT
};

}

inline const char* to_name( option o ) {
	return detail::option_names[(int)o];
}

}

#endif
