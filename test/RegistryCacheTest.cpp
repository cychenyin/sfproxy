/*
 * RegistryCacheTest.cpp
 *
 *  Created on: Mar 13, 2014
 *      Author: asdf
 */

#include "RegistryCacheTest.h"

namespace ut {

RegistryCacheTest::RegistryCacheTest() {
	port = 1;
}

RegistryCacheTest::~RegistryCacheTest() {
}

Registry RegistryCacheTest::createRegistry() {
	Registry r;  // = Registry("name", "host", ++port);
	r.name = "name";
	r.host = "host";
	r.port = ++port;
	return r;
}

void RegistryCacheTest::addTest() {
	Registry r = createRegistry();
	r.name = "add";
	cache.add(r);
	cache.add(r);
	cache.add(r);
	cache.add(r);
	cache.add(r);

	string name("add");
	vector<Registry> *v = cache.get(name);
	cout << " addTest get result size=" << v->size() << endl;

	v = cache.get(r.name);
	cout << " addTest get result size=" << v->size() << endl;

	if (v->begin() == v->end()) {
		cout << "nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn" << endl;
	}

	if (v && v->begin() != v->end()) {
		Registry &p = *(v->begin());
		cout << "name=" << p.name << "	host=" << p.host << "	port=" << p.port << "	w=" << p.weight() << endl;
	} else {
		cout << "fail to add or get" << endl;
	}
	cout << "2222222222222222222222222222222222222222222222222222222222222222222" << endl;
	r = createRegistry();
	r.name = "add2";
	cache.add(r);
	v = cache.get(r.name);
	cout << " addTest get result size=" << v->size() << endl;
	if (v && v->begin() != v->end()) {
		Registry &p = *(v->begin());
		cout << "name=" << p.name << "	host=" << p.host << "	port=" << p.port << "	w=" << p.weight() << endl;
	} else {
		cout << "fail to add or get" << endl;
	}
	cout << "333333333333333333333333333333333333333333333333333333333333333333" << endl;
	r = createRegistry();
	r.name = "add3";
	cache.add(r);
	cache.add(r);
	cache.add(r);
	v = cache.get(r.name);
	cout << " addTest get result size=" << v->size() << endl;
	if (v && v->begin() != v->end()) {
		Registry &p = *(v->begin());
		cout << "name=" << p.name << "	host=" << p.host << "	port=" << p.port << "	w=" << p.weight() << endl;
	} else {
		cout << "fail to add or get" << endl;
	}
}

void RegistryCacheTest::removeTest() {
}
void RegistryCacheTest::clearTest() {
}
void RegistryCacheTest::replaceTest() {
}
void RegistryCacheTest::emptyTest() {
}

} /* namespace ut */
