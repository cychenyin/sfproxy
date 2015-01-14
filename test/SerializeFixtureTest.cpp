#define BOOST_TEST_NO_MAIN

#include <iostream>
#include <sstream>
#include <boost/test/unit_test.hpp>
#include "../core/RegistryCache.h"
#include "../core/FileCache.h"
#include <stdio.h>

using namespace FinagleRegistryProxy;
using namespace std;

BOOST_AUTO_TEST_SUITE( serialize_fixture_suite )

const string filename = "/tmp/serialize.test";
const int count = 2;


class F {
	F() {
		// BOOST_TEST_MESSAGE("setup fixture");
		setup();
	}
	~F() {
		teardown();
	}

	RegistryCache* p;

	Registry createReg(int i) {
		Registry r;
		stringstream ss;
		ss << "servier" << i;
		r.name = ss.str();
		r.host = "localhost";
		r.port = i;
		r.ephemeral = "e";
		r.ctime = 1232344;
		return r;
	}

	void setup() {
		p = new RegistryCache();
		for (int i = 0; i < count; i++) {
			Registry r = createReg(i);
			p->add(r);
		}
		// BOOST_TEST_MESSAGE("cache ready");
	}

	void teardown() {
		if (p) {
			delete p;
			p = NULL;
		}
		// BOOST_TEST_MESSAGE("cache tear down");
	}

};

BOOST_FIXTURE_TEST_CASE( empty , F) {
	BOOST_CHECK(true);
}

BOOST_FIXTURE_TEST_CASE( unser_no_file, F) {
	remove(filename.c_str());
	p->clear();
	p->from_file(filename);


	BOOST_CHECK_EQUAL(p->size(), 0);
}

BOOST_FIXTURE_TEST_CASE( serialize , F) {
	p->save(filename);
	BOOST_CHECK(true);
}

BOOST_FIXTURE_TEST_CASE( unserialize , F) {
	setup();

	const string content =
			"[{\"name\":\"servier0\",\"host\":\"localhost\",\"port\":0,\"weight\":3600 },{\"name\":\"servier1\",\"host\":\"localhost\",\"port\":1,\"weight\":3600 }]";
	// vector<Registry>
	RVector v = Registry::unserialize(content);
	BOOST_CHECK_EQUAL(v.size(), 2);

}

BOOST_FIXTURE_TEST_CASE( ser_vs_unser , F) {
	assert(count > 0);

	p->save(filename);

	RegistryCache* up = new RegistryCache();
	up->from_file(filename);

	BOOST_CHECK_EQUAL(up->size(), count);

	Registry left = createReg(1);
	RVector* v = p->get(left.name);
	BOOST_CHECK_EQUAL(v->size(), 1);
	Registry right = v->front();
	BOOST_CHECK_EQUAL(left.name, right.name);
	BOOST_CHECK_EQUAL(left.host, right.host);
	BOOST_CHECK_EQUAL(left.port, right.port);

	left = createReg(count - 1);
	BOOST_CHECK_EQUAL(v->size(), 1);
	right = v->front();
	BOOST_CHECK_EQUAL(left.name, right.name);
	BOOST_CHECK_EQUAL(left.host, right.host);
	BOOST_CHECK_EQUAL(left.port, right.port);

	delete up;
	up = NULL;

	teardown();
}

BOOST_AUTO_TEST_SUITE_END()
