/*
 * TestList.h
 *
 *  Created on: Mar 13, 2014
 *      Author: asdf
 */

#ifndef TESTLIST_H_
#define TESTLIST_H_

#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <list>
#include <string>
#include <algorithm>

using namespace std;

class TestList {
public:
	TestList();
	virtual ~TestList();


	void listTest();

	static void print(string& str);
private:
	list<string> l;
};

#endif /* TESTLIST_H_ */
