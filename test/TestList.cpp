/*
 * TestList.cpp
 *
 *  Created on: Mar 13, 2014
 *      Author: asdf
 */

#include "TestList.h"


TestList::TestList() {

}

TestList::~TestList() {
	l.clear();
}

void TestList::print(string& str) {
	cout << str << endl;
}

void TestList::listTest() {
	l.push_back("one"); // one
	l.push_back("two"); // two one

	l.push_front("nine"); // two one nine
	l.push_front("eight"); // two one nine eight

	l.push_front("seven"); // two one nine eight
	list<string>::iterator it;

	cout << "size=" << l.size() << endl;
	it = find(l.begin(), l.end(), "oneX");

	if (it == l.end()) {
		cout << "not found" << endl;
	} else {
		cout << "found:" << *it << endl;
	}

	int ccc(0);
//	count(l.begin(), l.end(), "one", ccc);
	// cout << "" << ccc << endl;

	// eight, nine, one, two
	cout << "-------------------use for" << endl;
	for (it = l.begin(); it != l.end(); ++it) {
		cout << *it << endl;
	}
	cout << "-------------------use while" << endl;
	it = l.begin();
	while (it != l.end()) {
		cout << *it << endl;
		it++;
	}

	cout << "-------------------use for_each" << endl;
	for_each(l.begin(), l.end(), TestList::print);

	l.sort();

	cout << "-------------------use for_each sorted" << endl;
	for_each(l.begin(), l.end(), TestList::print);

}
//int main(void) {
//	cout << "Test map running..." << endl; /* prints !!!Hello World!!! */
//}
