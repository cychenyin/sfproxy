
#ifndef BOOST_TEST_FRPROXY_
#define BOOST_TEST_FRPROXY_



#include <boost/test/auto_unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>


BOOST_AUTO_UNIT_TEST (MyTestCase)
{
    float fnum = 2.00001f;

    BOOST_CHECK(x != 0.0f);
    BOOST_CHECK_EQUAL((int)x, 9);
    BOOST_CHECK_CLOSE(x, 9.5f, 0.0001f); // Checks differ no more then 0.0001%
}



#endif
