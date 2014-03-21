/*
 * JsonUtil.h
 *
 *  Created on: Mar 11, 2014
 *      Author: asdf
 */

#ifndef JSONUTIL_H_
#define JSONUTIL_H_

#include <stdio.h>
#include <string>
#include <iostream>

#include "../rapidjson/document.h"
#include "../rapidjson/prettywriter.h"
#include "../rapidjson/filestream.h"
#include "../rapidjson/stringbuffer.h"

#include "Registry.h"

using namespace std;
using namespace rapidjson;

namespace FinagleRegistryProxy {

class JsonUtil {

public:
	static Document Parse( string str);
	// static string serialize(Registry& reg);
};

} /* namespace FinagleRegistryProxy */

#endif /* JSONUTIL_H_ */
