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
// #include "JsonUtil.h"

namespace FinagleRegistryProxy {
using namespace  std;

class Registry {
public:
	// zk path really, eg. soa/serivces/rta.counter.thrift
	std::string name;
	std::string host;
	int port;
	// time read from zk stat, in ms, value = stat.ctime/ 1000
	uint64_t ctime;
	// update time in cache; normally, is read zk node timestamp, or read file timestamp
	uint64_t mtime;
	// zk ephemeral node name, eg. member00000001
	std::string ephemeral;
	std::string json;
public:
	Registry();
	Registry(string name , string host, int port);
	virtual ~Registry();

	// check validation
	bool check();
	// get service weight, in the hour following register hour, weight be the elapse seconds; after 1 hour, weight will always be 3600;
	int weight();
	bool equals(Registry& r);
	bool operator==(Registry& r);
	bool operator!=(Registry& r);

	static string serialize(Registry &r);
//	static string to_json_string1(Registry &r);
//	static string to_json_string2(Registry &r);
//	static string to_json_string3(Registry &r);
//	static string to_json_string4(Registry &r);

	static string serialize(vector<Registry>& v);

	static vector<Registry> unserialize(const string& json);
};

} /* namespace FinagleRegistryProxy */

#endif /* REGISTRY_H_ */
