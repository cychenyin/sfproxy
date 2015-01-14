#ifndef BOOST_TEST_FRPROXY_
#define BOOST_BTEST_FRPROXY_

//#include <boost/test/auto_unit_test.hpp>
//#include <boost/test/execution_monitor.hpp>
//#include <boost/test/floating_point_comparison.hpp>

//#define BOOST_TEST_DYN_LINK
//#define BOOST_TEST_MAIN
//#define BOOST_TEST_MODULE shit

//using namespace boost::unit_test;

namespace btest {
} // namespace btest

#include <boost/test/minimal.hpp>
int addb(int i, int j) {
	return i + j + 3;
}

int test_main(int, char *[])             // note the name!
		{
	// six ways to detect and report the same error:
	BOOST_CHECK(addb(2, 2) == 4);        // #1 continues on error
	BOOST_REQUIRE(addb(2, 2) == 4);      // #2 throws on error
	if (addb(2, 2) != 4)
		BOOST_ERROR("Ouch...");          // #3 continues on error
	if (addb(2, 2) != 4)
		BOOST_FAIL("Ouch...");           // #4 throws on error
	if (addb(2, 2) != 4)
		throw "Oops..."; // #5 throws on error

	return addb(2, 2) == 4 ? 0 : 1;       // #6 returns error code
}

//BOOST_AUTO_TEST_SUITE(minimal_test)
//
//BOOST_AUTO_TEST_CASE(a)
//{
//	BOOST_REQUIRE(true);
//    BOOST_CHECK(addc(2, 2) == 5);
//    BOOST_CHECK_EQUAL(1, 2);
//}
//
//
//BOOST_AUTO_TEST_SUITE_END()

#endif

