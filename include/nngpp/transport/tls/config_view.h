#ifndef NNGPP_TLS_CONFIG_VIEW_H
#define NNGPP_TLS_CONFIG_VIEW_H
#include <nngpp/error.h>
#include <nng/supplemental/tls/tls.h>

namespace nng { namespace tls {

enum class mode {
	client = NNG_TLS_MODE_CLIENT,
	server = NNG_TLS_MODE_SERVER
};

enum class auth_mode {
	none     = NNG_TLS_AUTH_MODE_NONE,
	optional = NNG_TLS_AUTH_MODE_OPTIONAL,
	required = NNG_TLS_AUTH_MODE_REQUIRED
};

enum class version {
	tls_1_0 = NNG_TLS_1_0,
	tls_1_1 = NNG_TLS_1_1,
	tls_1_2 = NNG_TLS_1_2,
	tls_1_3 = NNG_TLS_1_3
};

struct config;

struct config_view {
protected:
	nng_tls_config* c = nullptr;
	
public:
	config_view() = default;

	config_view( nng_tls_config* c ) noexcept : c(c) {}
	
	nng_tls_config* get() const noexcept {
		return c;
	}

	nng_tls_config* operator->() const noexcept {
		return c;
	}

	explicit operator bool() const noexcept {
		return c != nullptr;
	}

	config hold() const noexcept;
	
	const config_view& config_server_name( const char* name ) const {
		int r = nng_tls_config_server_name(c,name);
		if( r != 0 ) {
			throw exception(r,"nng_tls_config_server_name");
		}
		return *this;
	}
	
	const config_view& config_ca_chain( const char* chain, const char* crl = nullptr ) const {
		int r = nng_tls_config_ca_chain(c,chain,crl);
		if( r != 0 ) {
			throw exception(r,"nng_tls_config_ca_chain");
		}
		return *this;
	}
	
	const config_view& config_own_cert( const char* cert, const char* key, const char* pass = nullptr ) const {
		int r = nng_tls_config_own_cert(c,cert,key,pass);
		if( r != 0 ) {
			throw exception(r,"nng_tls_config_own_cert");
		}
		return *this;
	}
	
	const config_view& config_key( const uint8_t* data, size_t size ) const {
		int r = nng_tls_config_key(c,data,size);
		if( r != 0 ) {
			throw exception(r,"nng_tls_config_key");
		}
		return *this;
	}
	
	const config_view& config_pass( const char* pass ) const {
		int r = nng_tls_config_pass(c,pass);
		if( r != 0 ) {
			throw exception(r,"nng_tls_config_pass");
		}
		return *this;
	}
	
	const config_view& config_auth_mode( auth_mode mode ) const {
		int r = nng_tls_config_auth_mode(c,(nng_tls_auth_mode)mode);
		if( r != 0 ) {
			throw exception(r,"nng_tls_config_auth_mode");
		}
		return *this;
	}
	
	const config_view& config_ca_file( const char* path ) const {
		int r = nng_tls_config_ca_file(c,path);
		if( r != 0 ) {
			throw exception(r,"nng_tls_config_ca_file");
		}
		return *this;
	}
	
	const config_view& config_cert_key_file( const char* path, const char* pass = nullptr ) const {
		int r = nng_tls_config_cert_key_file(c,path,pass);
		if( r != 0 ) {
			throw exception(r,"nng_tls_config_cert_key_file");
		}
		return *this;
	}
	
	const config_view& config_version( version min_ver, version max_ver ) const {
		int r = nng_tls_config_version(c,(nng_tls_version)min_ver,(nng_tls_version)max_ver);
		if( r != 0 ) {
			throw exception(r,"nng_tls_config_version");
		}
		return *this;
	}

};

}}

#endif
