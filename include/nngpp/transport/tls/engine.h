#ifndef NNGPP_TLS_ENGINE_H
#define NNGPP_TLS_ENGINE_H
#include <nngpp/error.h>
#include <nng/supplemental/tls/engine.h>

namespace nng { namespace tls { namespace engine {

enum class version {
	v0 = NNG_TLS_ENGINE_V0,
	v1 = NNG_TLS_ENGINE_V1,
	version = NNG_TLS_ENGINE_VERSION
};

inline void register_(const nng_tls_engine* e) {
	int r = nng_tls_engine_register(e);
	if( r != 0 ) {
		throw exception(r,"nng_tls_engine_register");
	}
}

inline void send(void* arg, const uint8_t* buf, size_t* szp) {
	int r = nng_tls_engine_send(arg, buf, szp);
	if( r != 0 ) {
		throw exception(r,"nng_tls_engine_send");
	}
}

inline void recv(void* arg, uint8_t* buf, size_t* szp) {
	int r = nng_tls_engine_recv(arg, buf, szp);
	if( r != 0 ) {
		throw exception(r,"nng_tls_engine_recv");
	}
}

inline const char* name() noexcept {
	return nng_tls_engine_name();
}

inline const char* description() noexcept {
	return nng_tls_engine_description();
}

inline bool fips_mode() noexcept {
	return nng_tls_engine_fips_mode();
}

}}}

#endif
