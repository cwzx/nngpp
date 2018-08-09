#ifndef NNGPP_STAT_VIEW_H
#define NNGPP_STAT_VIEW_H
#include <nng/nng.h>

namespace nng {

enum class stat_type {
	level   = NNG_STAT_LEVEL,
	counter = NNG_STAT_COUNTER
};

enum class stat_unit {
	none     = NNG_UNIT_NONE,
	bytes    = NNG_UNIT_BYTES,
	messages = NNG_UNIT_MESSAGES,
	boolean  = NNG_UNIT_BOOLEAN,
	millis   = NNG_UNIT_MILLIS,
	events   = NNG_UNIT_EVENTS
};

struct stat_view {
protected:
	nng_stat* s = nullptr;
	
public:
	stat_view() = default;
	
	stat_view( nng_stat* s ) noexcept : s(s) {}
	
	nng_stat* get() const noexcept {
		return s;
	}

	nng_stat* operator->() const noexcept {
		return s;
	}

	explicit operator bool() const noexcept {
		return s != nullptr;
	}

	const char* name() const noexcept {
		return nng_stat_name(s);
	}

	stat_type type() const noexcept {
		return (stat_type)nng_stat_type(s);
	}

	stat_unit unit() const noexcept {
		return (stat_unit)nng_stat_unit(s);
	}

	int64_t value() const noexcept {
		return nng_stat_value(s);
	}

};

}

#endif
