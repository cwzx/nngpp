// This is a port of the nng demo to nngpp
// See https://github.com/nanomsg/nng/tree/master/demo/async

#include <cstdio>
#include <memory>
#include <nngpp/nngpp.h>
#include <nngpp/protocol/rep0.h>
#include <nngpp/platform/platform.h>

// Parallel is the maximum number of outstanding requests we can handle.
// This is *NOT* the number of threads in use, but instead represents
// outstanding work items.  Select a small number to reduce memory size.
// (Each one of these can be thought of as a request-reply loop.)  Note
// that you will probably run into limitations on the number of open file
// descriptors if you set this too high. (If not for that limit, this could
// be set in the thousands, each context consumes a couple of KB.)
#ifndef PARALLEL
#define PARALLEL 128
#endif

void server_cb(void* arg);

// The server keeps a list of work items, sorted by expiration time,
// so that we can use this to set the timeout to the correct value for
// use in poll.
struct work {
	enum { INIT, RECV, WAIT, SEND } state = INIT;
	nng::aio aio{ server_cb, this };
	nng::msg msg;
	nng::ctx ctx;

	explicit work( nng::socket_view sock ) : ctx(sock) {}
};

void server_cb(void* arg) {
	work* work = (struct work*)arg;
	uint32_t when;

	switch(work->state) {
	case work::INIT:
		work->state = work::RECV;
		work->ctx.recv( work->aio );
		break;
	case work::RECV:
		{
			auto result = work->aio.result();
			if( result != nng::error::success ) {
				throw nng::exception(result);
			}
		}
		{
			auto msg = work->aio.release_msg();
			try {
				when = msg.body().trim_u32();
			}
			catch( const nng::exception& ) {
				// bad message, just ignore it.
				work->ctx.recv( work->aio );
				return;
			}
			work->msg = std::move(msg);
		}
		work->state = work::WAIT;
		nng::sleep( when, work->aio );
		break;
	case work::WAIT:
		// We could add more data to the message here.
		work->aio.set_msg( std::move(work->msg) );
		work->state = work::SEND;
		work->ctx.send( work->aio );
		break;
	case work::SEND:
		{
			auto result = work->aio.result();
			if( result != nng::error::success ) {
				throw nng::exception(result);
			}
		}
		work->state = work::RECV;
		work->ctx.recv( work->aio );
		break;
	default:
		throw nng::exception(nng::error::state);
		break;
	}
}

// The server runs forever.
void server(const char* url) {
	//  Create the socket.
	auto sock = nng::rep::open();

	std::unique_ptr<work> works[PARALLEL];
	for(int i=0;i<PARALLEL;++i) {
		works[i] = std::unique_ptr<work>(new work(sock));
	}

	sock.listen(url);

	for(int i=0;i<PARALLEL;++i) {
		server_cb(works[i].get()); // this starts them going (INIT state)
	}

	while(true) {
		nng::msleep(3600000); // neither pause() nor sleep() portable
	}
}

int main(int argc, char** argv) try {
	if (argc != 2) {
		fprintf(stderr, "Usage: %s <url>\n", argv[0]);
		return 1;
	}
	server(argv[1]);
}
catch( const nng::exception& e ) {
	fprintf(stderr, "%s: %s\n", e.who(), e.what());
	return 1;
}
