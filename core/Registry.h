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

// #include "../rapidjson/document.h"
// #include "../rapidjson/prettywriter.h"
// #include "../rapidjson/filestream.h"
// #include "../rapidjson/stringbuffer.h"

#include "../frproxy.h"
#include "JsonUtil.h"

namespace FinagleRegistryProxy {
using namespace  std;

class Registry {
public:
	// zk path really
	std::string name;
	std::string host;
	int port;
	int ctime;
	std::string ephemeral;

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
	bool operator!=(Registry& r);

	static string to_json_string(Registry &r);
//	static string to_json_string1(Registry &r);
//	static string to_json_string2(Registry &r);
//	static string to_json_string3(Registry &r);
//	static string to_json_string4(Registry &r);

	static string to_json_string(vector<Registry> v);
};

} /* namespace FinagleRegistryProxy */

#endif /* REGISTRY_H_ */
