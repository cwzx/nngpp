#include <catch2/catch.hpp>
#include <cstring>
#include <nngpp/nngpp.h>
#include <nngpp/tcp/tcp.h>

namespace {
static uint8_t loopback[4] = { 127, 0, 0, 1 };
}

TEST_CASE("Supplemental TCP", "[tcpapi]") {
	
	INFO("We can create a dialer and listener");
	auto d = nng::tcp::make_dialer();
	auto l = nng::tcp::make_listener();
	REQUIRE(d);
	REQUIRE(l);
	
	INFO("Listener listens (wildcard)");
	uint32_t ip;
	memcpy(&ip, loopback, 4);

	nng_sockaddr sa;
	sa.s_in.sa_family = NNG_AF_INET;
	sa.s_in.sa_addr = ip;
	sa.s_in.sa_port = 0;

	REQUIRE_NOTHROW(l.listen(sa));
	REQUIRE_NOTHROW(sa = get_opt_local_address(l));
	REQUIRE(sa.s_in.sa_family == NNG_AF_INET);
	REQUIRE(sa.s_in.sa_addr == ip);
	REQUIRE(sa.s_in.sa_port != 0);

	INFO("We can dial it");
	nng::aio daio(nullptr,nullptr);
	nng::aio laio(nullptr,nullptr);
	nng::aio maio(nullptr,nullptr);

	d.dial(sa, daio);
	l.accept(laio);

	daio.wait();
	laio.wait();

	REQUIRE(daio.result() == nng::error::success);
	REQUIRE(laio.result() == nng::error::success);

	nng::tcp::tcp c1( daio.get_output<nng_tcp>(0) );
	nng::tcp::tcp c2( laio.get_output<nng_tcp>(0) );
	REQUIRE(c1);
	REQUIRE(c2);
	
	INFO("They exchange messages");
	nng::aio aio1(nullptr,nullptr);
	nng::aio aio2(nullptr,nullptr);

	REQUIRE_NOTHROW(set_opt_no_delay(c1,true));
	REQUIRE_NOTHROW(set_opt_no_delay(c2,true));
	REQUIRE_NOTHROW(set_opt_keep_alive(c1,true));
	
	char buf1[5];
	memcpy(buf1, "TEST", 5);

	char buf2[5];
	memset(buf2, 0, 5);
	
	aio1.set_iov( {buf1, 5} );
	aio2.set_iov( {buf2, 5} );
	c1.send(aio1);
	c2.recv(aio2);
	aio1.wait();
	aio2.wait();

	REQUIRE(aio1.result() == nng::error::success);
	REQUIRE(aio1.count() == 5);

	REQUIRE(aio2.result() == nng::error::success);
	REQUIRE(aio2.count() == 5);

	REQUIRE(memcmp(buf1, buf2, 5) == 0);

	INFO("Socket name matches");
	{
		nng_sockaddr sa2;
		REQUIRE_NOTHROW(sa2 = get_opt_local_address(c2));
		REQUIRE(sa2.s_in.sa_family == NNG_AF_INET);
		REQUIRE(sa2.s_in.sa_addr == ip);
		REQUIRE(sa2.s_in.sa_port == sa.s_in.sa_port);
	}
	
	INFO("Peer name matches")
	{
		nng_sockaddr sa2;
		REQUIRE_NOTHROW(sa2 = get_opt_remote_address(c1));
		REQUIRE(sa2.s_in.sa_family == NNG_AF_INET);
		REQUIRE(sa2.s_in.sa_addr == ip);
		REQUIRE(sa2.s_in.sa_port == sa.s_in.sa_port);
	}

}
