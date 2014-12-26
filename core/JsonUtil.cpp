/*
 * JsonUtil.cpp
 *
 *  Created on: Mar 11, 2014
 *      Author: asdf
 */

#include "JsonUtil.h"

namespace FinagleRegistryProxy {

vector<Registry> Parse(string str) {
	Document d;
	d.Parse<0>(str.c_str());

//	Registry reg;
//	reg.name = serviceZpath;
//	reg.ctime = stat.ctime / 1000;
//	reg.ephemeral = ephemeralName;
//
//	if (!d.HasParseError() && d.HasMember("serviceEndpoint")) {
//		const Value &v = d["serviceEndpoint"];
//		if (v.IsObject() && v.HasMember("host")) {
//			reg.host = v["host"].GetString();
//			if (v.HasMember("port")) {
//				reg.port = v["port"].GetInt();
//			}
//		}
//	}

}

string serialize(Registry& reg) {

	return 0;
}

} /* namespace FinagleRegistryProxy */
