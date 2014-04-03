/*
 ============================================================================
 Name        : autotoolsTest.cpp
 Author      : nobody
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C++,
 ============================================================================
 */

#include <iostream>


#include "../frpoxy.h"

#include "RegistryCacheTest.h"
#include "ZkClientTest.h"
#include "ListTest.h"

#include "../core/ServerHandler.cpp"

using namespace std;
using namespace ut;

int main(int argc, char** argv) {
	cout << "welcome to finagle regsitry proxy tester" << endl;

//	ut::RegistryCacheTest a;
//	a.addTest();

	ZkClientTest c;
//	c.ConnecionTest(); // done
//	c.RegistryEqualsTest(); //done
//	c.poolIncTest(); // done
//	c.poolMemReleaseTest(); // done
//	c.poolTimeoutTest();
	c.clientDestoryOnDisconnectTest();
	cout << "end........................................................................." << endl;
	return 0;
}
