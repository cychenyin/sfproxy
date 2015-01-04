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
#include <boost/version.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/convenience.hpp>

#include "Registry.h"

using namespace std;
using namespace rapidjson;
using boost::shared_ptr;

namespace FinagleRegistryProxy {

using namespace FinagleRegistryProxy;

class JsonUtil {

public:
	// vector<Registry> Parse( string str);
};

} /* namespace FinagleRegistryProxy */

#endif /* JSONUTIL_H_ */
