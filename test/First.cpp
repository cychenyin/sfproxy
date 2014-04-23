/*
 * First.cpp
 *
 *  Created on: Mar 14, 2014
 *      Author: asdf
 */

#include <iostream>
#include <string>
#include "First.h"
#include "../frproxy.h"
#include "ganji/util/log/scribe_log.h"

using namespace std;

namespace ut {

First::First() {
}

First::~First() {
	ganji::util::log::ThriftLog::LogUninit();
}

int First::GetTwo() {
	return 2;
}


} /* namespace ut */


int main(int argc, char** argv) {
	cout << "" << endl;
	string host = "127.0.0.1";
	ganji::util::log::ThriftLog::LogInit(host.c_str(), 11463, 999999);

}
