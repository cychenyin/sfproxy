/*
 * JsonUtil.cpp
 *
 *  Created on: Mar 11, 2014
 *      Author: asdf
 */

#include "JsonUtil.h"

namespace FinagleRegistryProxy {

Document Parse( string str) {

	Document d;
	d.Parse<0>(str.c_str());

	if(!d.HasParseError() && d.HasMember("serviceEndpoint") ) {

		Registry r;
	}

	return 0;
}

string serialize(Registry& reg) {

	return 0;
}


} /* namespace FinagleRegistryProxy */
