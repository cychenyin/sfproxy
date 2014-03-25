/*
 * RegistryCacheTest.h
 *
 *  Created on: Mar 13, 2014
 *      Author: asdf
 */

#ifndef REGISTRYCACHETEST_H_
#define REGISTRYCACHETEST_H_

#include <iostream>

#include "../core/RegistryCache.h"

using namespace std;

namespace ut {
using namespace FinagleRegistryProxy;

class RegistryCacheTest {
public:
	RegistryCacheTest();
	virtual ~RegistryCacheTest();

	void addTest();
	void removeTest();
	void clearTest();
	void replaceTest();
	void emptyTest();
private:
	int port;
	RegistryCache cache;
	Registry createRegistry();
};

} /* namespace ut */

#endif /* REGISTRYCACHETEST_H_ */
