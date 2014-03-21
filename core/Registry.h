/*
 * Registry.h
 *
 *  Created on: Mar 17, 2014
 *      Author: asdf
 */

#ifndef REGISTRY_H_
#define REGISTRY_H_

#include <string>
#include <vector>

#include "../rapidjson/document.h"
#include "../rapidjson/prettywriter.h"
#include "../rapidjson/filestream.h"
#include "../rapidjson/stringbuffer.h"

#include "JsonUtil.h"

namespace FinagleRegistryProxy {
using namespace  std;

class Registry {
public:
	std::string name;
	std::string host;
	int port;
	int ctime;
	Registry();
	Registry(string name , string host, int port);
	virtual ~Registry();

	std::string json;
	// check validation
	bool check();
	// get service weight, in the hour following register hour, weight be the elapse seconds; after 1 hour, weight will always be 3600;
	int weight();
	bool equals(Registry& r);
	bool operator==(Registry& r);

	static string toJsonString(Registry &r);
	static string toJsonStringx(vector<Registry> v);
};

} /* namespace FinagleRegistryProxy */

#endif /* REGISTRY_H_ */
