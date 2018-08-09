#ifndef NNGPP_SNAPSHOT_VIEW_H
#define NNGPP_SNAPSHOT_VIEW_H
#include "stat_view.h"
#include "error.h"

namespace nng {

struct snapshot_view {
protected:
	nng_snapshot* s = nullptr;
	
public:
	snapshot_view() = default;
	
	snapshot_view( nng_snapshot* s ) noexcept : s(s) {}
	
	nng_snapshot* get() const noexcept {
		return s;
	}

	nng_snapshot* operator->() const noexcept {
		return s;
	}

	explicit operator bool() const noexcept {
		return s != nullptr;
	}

	void update() const {
		int r = nng_snapshot_update(s);
		if( r != 0 ) {
			throw exception(r,"nng_snapshot_update");
		}
	}
	
	void next( stat_view& stat ) const {
		auto st = stat.get();
		int r = nng_snapshot_next(s,&st);
		if( r != 0 ) {
			throw exception(r,"nng_snapshot_next");
		}
		stat = stat_view(st);
	}

};

}

#endif
