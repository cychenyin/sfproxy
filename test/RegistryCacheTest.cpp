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
	r.host ="host";
	r.port  = ++port;
	return r;
}

void RegistryCacheTest::addTest() {
	cout << "add test" << endl;
	Registry r = createRegistry();
	r.name = "add";
	cache = RegistryCache();
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
