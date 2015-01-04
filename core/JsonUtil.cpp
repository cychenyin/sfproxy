/*
 * JsonUtil.cpp
 *
 *  Created on: Mar 11, 2014
 *      Author: asdf
 */

#include "JsonUtil.h"

namespace FinagleRegistryProxy {

//vector<Registry> Parse(string str) {
//	vector<Registry> v;
//	Document d;
//	d.Parse<0>(str.c_str());
//	if(d.IsArray()) {
//		for(int i=0; i < d.Size(); i++ ) {
//			if(d[i].HasMember("name")  && d[i].HasMember("host") && d[i].HasMember("port")) {
//				Registry reg;
//				reg.name =  d[i]["name"].GetString();
//				reg.ctime = utils::now_in_ms();
//				reg.host = d[i]["host"].GetString();
//				reg.host = d[i]["port"].GetInt();
//				reg.ephemeral = "";
//				v.push_back(reg);
//			}
//		}
//	}
//	return v;
//}

} /* namespace FinagleRegistryProxy */
