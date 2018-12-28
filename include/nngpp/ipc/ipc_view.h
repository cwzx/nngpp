#ifndef NNGPP_IPC_IPC_VIEW_H
#define NNGPP_IPC_IPC_VIEW_H
#include <nng/supplemental/ipc/ipc.h>
#include <nngpp/aio_view.h>
#include <nngpp/transport/ipc.h>

namespace nng { namespace ipc {

struct ipc_view {
protected:
	nng_ipc* c = nullptr;
	
public:
	ipc_view() = default;
	
	ipc_view( nng_ipc* c ) noexcept : c(c) {}

	nng_ipc* get() const noexcept {
		return c;
	}

	nng_ipc* operator->() const noexcept {
		return c;
	}

	explicit operator bool() const noexcept {
		return c != nullptr;
	}

	void close() const noexcept {
		nng_ipc_close(c);
	}
	
	void send( aio_view a ) const noexcept {
		nng_ipc_send(c,a.get());
	}
	
	void recv( aio_view a ) const noexcept {
		nng_ipc_recv(c,a.get());
	}
	
	void get_opt( const char* name, void* val, size_t* szp ) const {
		int r = nng_ipc_getopt(c,name,val,szp);
		if( r != 0 ) {
			throw exception(r,"nng_ipc_getopt");
		}
	}
	
	uint64_t get_opt_uint64( const char* name ) const {
		uint64_t out;
		size_t size = sizeof(out);
		get_opt(name,&out,&size);
		return out;
	}
	
	nng_sockaddr get_opt_sockaddr( const char* name ) const {
		nng_sockaddr out;
		size_t size = sizeof(out);
		get_opt(name,&out,&size);
		return out;
	}
	
	void set_opt( const char* name, const void* val, size_t sz ) const {
		int r = nng_ipc_setopt(c,name,val,sz);
		if( r != 0 ) {
			throw exception(r,"nng_ipc_setopt");
		}
	}
	
};

inline nng_sockaddr get_opt_local_address( ipc_view c ) {
	return c.get_opt_sockaddr( to_name(nng::option::local_address) );
}

inline nng_sockaddr get_opt_remote_address( ipc_view c ) {
	return c.get_opt_sockaddr( to_name(nng::option::remote_address) );
}

inline uint64_t get_opt_peer_uid( ipc_view c ) {
	return c.get_opt_uint64( to_name(option::peer_uid) );
}

inline uint64_t get_opt_peer_gid( ipc_view c ) {
	return c.get_opt_uint64( to_name(option::peer_gid) );
}

inline uint64_t get_opt_peer_pid( ipc_view c ) {
	return c.get_opt_uint64( to_name(option::peer_pid) );
}

inline uint64_t get_opt_peer_zoneid( ipc_view c ) {
	return c.get_opt_uint64( to_name(option::peer_zoneid) );
}

}}

#endif
