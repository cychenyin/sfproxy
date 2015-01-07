#define BOOST_TEST_NO_MAIN

#include <boost/test/unit_test.hpp>
#include "../core/RegistryCache.h"
#include "../core/FileCache.h"

using namespace FinagleRegistryProxy;
using namespace std;

BOOST_AUTO_TEST_SUITE( serialize_suite )

RegistryCache* p;
void setup(){

}

void teardown() {

}

BOOST_AUTO_TEST_CASE( serialize ) {
	setup();

	teardown();
}

BOOST_AUTO_TEST_CASE( unserialize ) {
	setup();

	teardown();
}

BOOST_AUTO_TEST_CASE( save ) {
	setup();

	teardown();
}

BOOST_AUTO_TEST_CASE( read ) {
	setup();

	teardown();
}

BOOST_AUTO_TEST_CASE( ff_test ) {
	// seven ways to detect and report the same error:
	BOOST_CHECK(4 == 4);        // #1 continues on error

	BOOST_REQUIRE(4 == 4);      // #2 throws on error

	if (4 != 4)
		BOOST_ERROR("Ouch...");            // #3 continues on error

	if (4 != 4)
		BOOST_FAIL("Ouch...");             // #4 throws on error

	if (4 != 4)
		throw "Ouch..."; // #5 throws on error

	BOOST_CHECK_MESSAGE(4 == 4,  // #6 continues on error
			"addf(..) result: " << 4);

	BOOST_CHECK_EQUAL(4, 4);	  // #7 continues on error
}

BOOST_AUTO_TEST_SUITE(filecache_suite)
