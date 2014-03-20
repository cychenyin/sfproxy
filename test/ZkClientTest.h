/*
 * ZkClientTest.h
 *
 *  Created on: Mar 19, 2014
 *      Author: asdf
 */

#ifndef ZKCLIENTTEST_H_
#define ZKCLIENTTEST_H_

#include <iostream>

#include "../core/ZkClient.h"

namespace ut {

using namespace FinagleRegistryProxy;

class ZkClientTest {
public:
	ZkClientTest();
	virtual ~ZkClientTest();

	void ConnecionTest();

};
} /* namespace ut */

#endif /* ZKCLIENTTEST_H_ */
