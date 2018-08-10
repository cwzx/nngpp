// This is a port of nng perf to nngpp
// See https://github.com/nanomsg/nng/tree/master/perf

// perf implements the same performance tests found in the standard
// nanomsg & mangos performance tests.  As with mangos, the decision
// about which test to run is determined by the program name (ARGV[0}])
// that it is run under.
//
// Options are:
//
// - remote_lat - remote latency side (client, aka latency_client)
// - local_lat  - local latency side (server, aka latency_server)
// - local_thr  - local throughput side
// - remote_thr - remote throughput side
// - inproc_lat - inproc latency
// - inproc_thr - inproc throughput
//
#include <nngpp/nngpp.h>
#include <cctype>
#include <cstdarg>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <nngpp/platform/platform.h>
#include <nngpp/protocol/pair1.h>

static void die(const char* fmt, ...) {
	va_list ap;

	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);
	fprintf(stderr, "\n");
	exit(2);
}

void latency_client(const char* addr, size_t msgsize, int trips) {
	nng_time start, end;
	{
		auto s = nng::pair::open();
		s.dial(addr);
		nng::msg msg(msgsize);

		start = nng::clock();
		for(int i = 0; i < trips; ++i) {
			s.send( std::move(msg) );
			msg = s.recv_msg();
		}
		end = nng::clock();
	}

	auto total   = (float)(end - start) / 1000;
	auto latency = (float)(total * 1000000) / (trips * 2);
	printf("total time: %.3f [s]\n", total);
	printf("message size: %d [B]\n", (int)msgsize);
	printf("round trip count: %d\n", trips);
	printf("average latency: %.3f [us]\n", latency);
}

void latency_server(const char* addr, size_t msgsize, int trips) {
	auto s = nng::pair::open();
	s.listen(addr);
	for(int i = 0; i < trips; ++i) {
		auto msg = s.recv_msg();
		if( msg.body().size() != msgsize ) {
			die("wrong message size: %d != %d", msg.body().size(), msgsize);
		}
		s.send( std::move(msg) );
	}

	// Wait a bit for things to drain... linger should do this.
	// 100ms ought to be enough.
	nng::msleep(100);
}

// Our throughput story is quite a mess.  Mostly I think because of the poor
// caching and message reuse.  We should probably implement a message pooling
// API somewhere.

void throughput_server(const char* addr, size_t msgsize, int count) {
	nng_time start, end;
	{
		auto s = nng::pair::open();
		nng::set_opt_recv_buffer( s, 128 );
		s.listen(addr);
		// Receive first synchronization message.
		s.recv_msg();

		start = nng::clock();

		for(int i = 0; i < count; ++i) {
			auto msg = s.recv_msg();
			if(msg.body().size() != msgsize) {
				die("wrong message size: %d != %d", msg.body().size(), msgsize);
			}
		}
		end = nng::clock();
		// Send a synchronization message (empty) to the other side,
		// and wait a bit to make sure it goes out the wire.
		s.send( nng::view("",0) );
		nng::msleep(200);
	}
	auto total     = (float)(end - start) / 1000;
	auto msgpersec = (float)count / total;
	auto mbps      = (float)(msgpersec * 8 * msgsize) / (1024 * 1024);
	printf("total time: %.3f [s]\n", total);
	printf("message size: %d [B]\n", (int)msgsize);
	printf("message count: %d\n", count);
	printf("throughput: %.f [msg/s]\n", msgpersec);
	printf("throughput: %.3f [Mb/s]\n", mbps);
}

void throughput_client(const char* addr, size_t msgsize, int count) {
	// We send one extra zero length message to start the timer.
	++count;
	auto s = nng::pair::open();
	nng::set_opt_send_buffer( s, 128 );
	nng::set_opt_recv_timeout( s, 5000 );
	s.dial(addr);
	s.send( nng::msg((size_t)0) );

	for(int i = 0; i < count; ++i) {
		s.send( nng::msg(msgsize) );
	}
	// Attempt to get the completion indication from the other side.
	s.recv_msg();
}

static int parse_int(const char* arg, const char* what) {
	char* eptr;
	auto val = strtol(arg, &eptr, 10);
	// Must be a postive number less than around a billion.
	if((val < 0) || (val > (1 << 30)) || (*eptr != 0) || (eptr == arg)) {
		die("Invalid %s", what);
	}
	return (int)val;
}

void do_local_lat(int argc, char** argv) {
	if(argc != 3) {
		die("Usage: local_lat <listen-addr> <msg-size> <roundtrips>");
	}

	auto msgsize = parse_int(argv[1], "message size");
	auto trips   = parse_int(argv[2], "round-trips");

	latency_server(argv[0], msgsize, trips);
}

void do_remote_lat(int argc, char** argv) {
	if(argc != 3) {
		die("Usage: remote_lat <connect-to> <msg-size> <roundtrips>");
	}

	auto msgsize = parse_int(argv[1], "message size");
	auto trips   = parse_int(argv[2], "round-trips");

	latency_client(argv[0], msgsize, trips);
}

void do_local_thr(int argc, char** argv) {
	if(argc != 3) {
		die("Usage: local_thr <listen-addr> <msg-size> <count>");
	}

	auto msgsize = parse_int(argv[1], "message size");
	auto trips   = parse_int(argv[2], "count");

	throughput_server(argv[0], msgsize, trips);
}

void do_remote_thr(int argc, char** argv) {
	if(argc != 3) {
		die("Usage: remote_thr <connect-to> <msg-size> <count>");
	}

	auto msgsize = parse_int(argv[1], "message size");
	auto trips   = parse_int(argv[2], "count");

	throughput_client(argv[0], msgsize, trips);
}

struct inproc_args {
	int         count;
	int         msgsize;
	const char* addr;
	void (*func)(const char*, size_t, int);
};

static void do_inproc(void* args) {
	auto ia = (inproc_args*)args;
	ia->func(ia->addr, ia->msgsize, ia->count);
}

void do_inproc_lat(int argc, char** argv) {
	if(argc != 2) {
		die("Usage: inproc_lat <msg-size> <count>");
	}

	inproc_args ia;
	ia.addr    = "inproc://latency_test";
	ia.msgsize = parse_int(argv[0], "message size");
	ia.count   = parse_int(argv[1], "count");
	ia.func    = latency_server;

	nng::thread thr( do_inproc, &ia );

	// Sleep a bit.
	nng::msleep(100);

	latency_client("inproc://latency_test", ia.msgsize, ia.count);
}

void do_inproc_thr(int argc, char** argv) {
	if(argc != 2) {
		die("Usage: inproc_thr <msg-size> <count>");
	}

	inproc_args ia;
	ia.addr    = "inproc://tput_test";
	ia.msgsize = parse_int(argv[0], "message size");
	ia.count   = parse_int(argv[1], "count");
	ia.func    = throughput_server;

	nng::thread thr( do_inproc, &ia );

	// Sleep a bit.
	nng::msleep(100);

	throughput_client("inproc://tput_test", ia.msgsize, ia.count);
}

bool matches(const char* arg, const char* name) {
	const char* ptr = arg;
	const char* x;

	while(
		((x = strchr(ptr,'/')) != nullptr) ||
	    ((x = strchr(ptr,'\\')) != nullptr) ||
	    ((x = strchr(ptr,':')) != nullptr)
	) {
		ptr = x + 1;
	}
	while(true) {
		if(*name == '\0') {
			break;
		}
		if(tolower(*ptr) != *name) {
			return false;
		}
		++ptr;
		++name;
	}

	switch(*ptr) {
	case '\0':
		return true;
	case '.': // extension; ignore it.
		return true;
	default: // some other trailing bit.
		return false;
	}
}

int main(int argc, char** argv) {
	char* prog;

	// Allow -m <remote_lat> or whatever to override argv[0].
	if(argc >= 3 && strcmp(argv[1], "-m") == 0) {
		prog = argv[2];
		argv += 3;
		argc -= 3;
	}
	else {
		prog = argv[0];
		--argc;
		++argv;
	}
	if(matches(prog, "remote_lat") || matches(prog, "latency_client")) {
		do_remote_lat(argc, argv);
	} 
	else if(matches(prog, "local_lat") || matches(prog, "latency_server")) {
		do_local_lat(argc, argv);
	} 
	else if(matches(prog, "local_thr") || matches(prog, "throughput_server")) {
		do_local_thr(argc, argv);
	} 
	else if(matches(prog, "remote_thr") || matches(prog, "throughput_client")) {
		do_remote_thr(argc, argv);
	} 
	else if(matches(prog, "inproc_thr")) {
		do_inproc_thr(argc, argv);
	} 
	else if(matches(prog, "inproc_lat")) {
		do_inproc_lat(argc, argv);
	} 
	else {
		die("Unknown program mode? Use -m <mode>.");
	}
}
