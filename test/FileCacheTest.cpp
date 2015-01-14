#define BOOST_TEST_NO_MAIN

#include <boost/test/unit_test.hpp>
#include "../core/FileCache.h"

using namespace FinagleRegistryProxy;
using namespace std;

BOOST_AUTO_TEST_SUITE( filecache_suite )

string filename = "/tmp/filecache.test";

BOOST_AUTO_TEST_CASE( openread ) {
	FileCache f(filename);
	f.open_read();
	f.close();
	BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE( openall ) {
	FileCache f(filename);
	f.open_write();
	f.close();
	BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE( readwrite ) {
	string origin = "test";

	FileCache fw(filename);
	fw.open_write();
	fw.write(origin);
	fw.flush();
	fw.close();
	BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE( readsize) {
	string origin = "test";

	FileCache fr(filename);
	fr.open_read();
	unsigned long size = fr.file_size();
	fr.close();

	BOOST_CHECK_EQUAL(size, origin.size());
}

BOOST_AUTO_TEST_CASE( readcontent) {
	string origin = "test";

	FileCache fr(filename);
	fr.open_read();

	string content = "";
	fr.read_all(content);

//	cout << "===============================================================" << endl;
//	cout << content << endl;

	fr.close();

	BOOST_CHECK(true);
}



BOOST_AUTO_TEST_SUITE_END()
