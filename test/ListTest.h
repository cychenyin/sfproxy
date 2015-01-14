/*
 * TestList.cpp
 *
 *  Created on: Mar 13, 2014
 *      Author: asdf
 */

#ifndef LISTTEST_H_
#define LISTTEST_H_

#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <list>
#include <string>
#include <algorithm>

using namespace std;
namespace ut {

class TestList {

private:
	list<string> l;
public:
	TestList() {

	}

	~TestList() {
		l.clear();
	}

	static void print(string& str) {
		cout << str << endl;
	}

	void itoratorTest2() {
//		l.push_back("1");
//		l.push_back("1");
//
//		for(list<string>::iterator it = l.begin(); it != l.end(); it++ ){
//			list<string>::iterator tmp = it;
//			cout << (&tmp == &it) << endl;
//			cout << *it << endl;
//			l.erase(tmp) ;
//		}

		// cout << "endddddddddddddddddddddddddddddddd" << endl;
		// result: throw exception when ++ after erase
	}
	void itoratorTest() {
		l.push_back("1"); // one
		l.push_back("2"); // two one
		l.push_back("3"); // two one
		l.push_back("4"); // two one
		l.push_back("5"); // two one
		l.push_back("6"); // two one
		l.push_back("7"); // two one
		l.push_back("8"); // two one
		l.push_back("9"); // two one
		l.push_back("10"); // two one
		list<string>::iterator it = l.begin();

		while (it != l.end()) {
			cout << *it << endl;
			list<string>::iterator t = it;
			++it;
			cout << "it == t :" << (it == t) << endl;
			l.erase(t);
		}

		cout << "--------------------------------------------- be " << l.size()  << endl;
		while (it != l.end()) {
			cout << *it << endl;
			++it;
		}
		cout << "--------------------------------------------- end" << endl;
		// result: iterator will work if list changed while iterator in use.
	}
	void listTest() {
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
		for_each(l.begin(), l.end(), print);

		l.sort();

		cout << "-------------------use for_each sorted" << endl;
		for_each(l.begin(), l.end(), print);

	}

};
// class
//int main(void) {
//	cout << "Test map running..." << endl; /* prints !!!Hello World!!! */
//}

}// namespace

#endif
