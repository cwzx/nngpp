#ifndef NNGPP_ERROR_H
#define NNGPP_ERROR_H
#include <nng/nng.h>
#include <system_error>

namespace nng {

enum class error {
	success     = 0,
	intr        = NNG_EINTR,
	nomem       = NNG_ENOMEM,
	inval       = NNG_EINVAL,
	busy        = NNG_EBUSY,
	timedout    = NNG_ETIMEDOUT,
	connrefused = NNG_ECONNREFUSED,
	closed      = NNG_ECLOSED,
	again       = NNG_EAGAIN,
	notsup      = NNG_ENOTSUP,
	addrinuse   = NNG_EADDRINUSE,
	state       = NNG_ESTATE,
	noent       = NNG_ENOENT,
	proto       = NNG_EPROTO,
	unreachable = NNG_EUNREACHABLE,
	addrinval   = NNG_EADDRINVAL,
	perm        = NNG_EPERM,
	msgsize     = NNG_EMSGSIZE,
	connaborted = NNG_ECONNABORTED,
	connreset   = NNG_ECONNRESET,
	canceled    = NNG_ECANCELED,
	nofiles     = NNG_ENOFILES,
	nospc       = NNG_ENOSPC,
	exist       = NNG_EEXIST,
	readonly    = NNG_EREADONLY,
	writeonly   = NNG_EWRITEONLY,
	crypto      = NNG_ECRYPTO,
	peerauth    = NNG_EPEERAUTH,
	noarg       = NNG_ENOARG,
	ambiguous   = NNG_EAMBIGUOUS,
	badtype     = NNG_EBADTYPE,
	connshut    = NNG_ECONNSHUT,
	internal    = NNG_EINTERNAL,
	syserr      = NNG_ESYSERR,
	tranerr     = NNG_ETRANERR
};

inline const char* to_string( error e ) noexcept {
	return nng_strerror( (int)e );
}

class error_category : public std::error_category {
public:
	static const error_category& get()    {static error_category e; return e;}

	~error_category() final {}

	const char *name() const noexcept final {
		return "nng";
	}

	std::string message(int code) const final {
		return nng_strerror(code);
	}

	std::error_condition default_error_condition(int code) const noexcept final {
		// Transport- and system-specific
		if (code & NNG_ESYSERR) {
			// Attempt to translate system-specific error
			return std::system_category().default_error_condition(code & ~int(NNG_ESYSERR));
		}
		switch ((error) code) {
			// Most NNG codes have a corresponding POSIX code and hence std::erc.
		case error::success:     return std::errc(0);
		case error::intr:        return std::errc::interrupted;
		case error::nomem:       return std::errc::not_enough_memory;
		case error::inval:       return std::errc::invalid_argument;
		case error::busy:        return std::errc::device_or_resource_busy;
		case error::timedout:    return std::errc::timed_out;
		case error::connrefused: return std::errc::connection_refused;
		case error::again:       return std::errc::resource_unavailable_try_again;
		case error::notsup:      return std::errc::not_supported;
		case error::addrinuse:   return std::errc::address_in_use;
		case error::noent:       return std::errc::no_such_file_or_directory;
		case error::proto:       return std::errc::protocol_error;
		case error::perm:        return std::errc::operation_not_permitted;
		case error::msgsize:     return std::errc::message_size;
		case error::connaborted: return std::errc::connection_aborted;
		case error::connreset:   return std::errc::connection_reset;
		case error::canceled:    return std::errc::operation_canceled;
		case error::nospc:       return std::errc::no_space_on_device;
		case error::exist:       return std::errc::file_exists;
			// These NNG errors have approximate 1:1 mappings to POSIX codes.
		case error::unreachable: return std::errc::host_unreachable;
		case error::addrinval:   return std::errc::address_not_available;
		case error::state:       return std::errc::inappropriate_io_control_operation;
		case error::closed:      return std::errc::bad_file_descriptor;
		case error::nofiles:     return std::errc::too_many_files_open;
			// These NNG errors have a "lossy" N-to-1 mapping to POSIX codes.
		case error::readonly:    return std::errc::permission_denied;
		case error::writeonly:   return std::errc::permission_denied;
		case error::crypto:      return std::errc::permission_denied;
		case error::peerauth:    return std::errc::permission_denied;
		case error::noarg:       return std::errc::invalid_argument;
		case error::ambiguous:   return std::errc::invalid_argument;
		case error::badtype:     return std::errc::invalid_argument;
		case error::connshut:    return std::errc::connection_aborted;
			// Continue using this category for unknown, internal and protocol errors.
		default: return std::error_condition(code, get());
		}
	}
};

class exception : public std::system_error {
	const char* source;
public:
	
	explicit exception( int e, const char* s = "" ) noexcept : std::system_error(e,error_category::get()), source(s) {}

	explicit exception( error e, const char* s = "" ) noexcept : exception((int)e,s) {}
	
	error get_error() const noexcept {
		return (error) code().value();
	}

	const char* who() const noexcept {
		return source;
	}
	
	const char* what() const noexcept final {
		return nng_strerror(code().value());
	}
};

}

#endif
