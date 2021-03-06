/*
 * Registry.cpp
 *
 *  Created on: Mar 17, 2014
 *      Author: asdf
 */

#include "Registry.h"
#include <sstream>
#include <stdlib.h>
#include <boost/format.hpp>
#include "../rapidjson/document.h"
#include "../rapidjson/prettywriter.h"
#include "../rapidjson/filestream.h"
#include "../rapidjson/stringbuffer.h"
#include "../log/logger.h"

using namespace rapidjson;

namespace FinagleRegistryProxy {

Registry::Registry() {
	this->name = "";
	this->host = "";
	this->port = 0;
	ctime = 0;
	mtime = utils::now_in_ms();
}

Registry::Registry(string name, string host, int port) {
	this->name = name;
	this->host = host;
	this->port = port;
	ctime = 0;
	mtime = utils::now_in_ms();
}
Registry::~Registry() {
	this->port = 0;
}

bool Registry::check() {
	return !name.empty() && !host.empty() && port > 0;
}

int Registry::weight() {
	long elapse = time(0) - ctime;
//	return elapse < 3600 && elapse > 0 ? elapse : 3600;
	// python: return 1200 + x * x / 36 if x < 294 and x > 0 else 3600
	return elapse < 294 && elapse > 0 ? 1200 + elapse * elapse / 36 : 3600;
}

bool Registry::equals(Registry& r) {
	return this == &r ? true : name == r.name && host == r.host && port == r.port;
}

bool Registry::operator==(Registry& r) {
	return this == &r ? true : name == r.name && host == r.host && port == r.port;
}

bool Registry::operator!=(Registry& r) {
	return !(this == &r ? true : name == r.name && host == r.host && port == r.port);
}

// 67us
// use stringstream, perfermance import 100% than using rapidJson
// 	eg. {"name":"/soa/services/zhaopin.company.http","host":"g1-zp-job-00.dns.ganji.com","port":28008,"weight":3600 }
string Registry::serialize(Registry& r) {
	stringstream ss;
	ss << "{\"name\":\"" << r.name;
	ss << "\",\"host\":\"" << r.host;
	ss << "\",\"port\":" << r.port;
	ss << ",\"weight\":" << r.weight();
	ss << " }";
	return ss.str();
}

// // 80-100us
// // use stringstream, perfermance import 100% than using rapidJson
// string Registry::to_json_string2(Registry& r) {
// 
// 	stringstream s;
// 	s << r.port;
// 	stringstream w;
// 	w << r.weight();
// 
// 	string ss;
// 	ss += "{\"name\":\"";
// 	ss += r.name;
// 	ss += "\",\"host\":\"";
// 	ss += r.host;
// 	ss += "\",\"port\":";
// 	ss += s.str();
// 	ss += ",\"weight\":";
// 	ss += w.str();
// 	ss += "}";
// 	return ss;
// }
// 
// // 120us
// string Registry::to_json_string3(Registry& r) {
// 	Document document;
// 	Document::AllocatorType& allocator = document.GetAllocator();
// 
// 	Value root(kObjectType);
// 	{
// 		Value jname(r.name.c_str());
// 		Value jhost(r.host.c_str());
// 		Value je(r.ephemeral.c_str());
// 		Value jport(r.port);
// 		Value jweight(r.weight());
// 		root.AddMember("name", jname, allocator);
// 		root.AddMember("host", jhost, allocator);
// 		root.AddMember("port", jport, allocator);
// //		root.AddMember("e", je, allocator);
// 		root.AddMember("weight", jweight, allocator);
// 	}
// 
// 	StringBuffer buffer;
// 	Writer<StringBuffer> writer(buffer);
// 	root.Accept(writer);
// 	return buffer.GetString();
// }

// //210us
// string Registry::to_json_string4(Registry& r) {
// 	boost::format fmt("{\"name\":\"%s\",\"host\":\"%s\",\"port\":%i,\"weight\":%i}");
// 	fmt % r.name % r.host % r.port % r.weight();
// 	return fmt.str();
// }
// return json format, eg. [{"name":"/soa/services/zhaopin.company.http","host":"g1-zp-job-00.dns.ganji.com","port":28008,"weight":3600 }]
string Registry::serialize(vector<Registry>& v) {
	string ret;
	ret += "[";
	vector<Registry>::iterator it = v.begin();
	while (it != v.end()) {
		if (ret.size() > 2) {
			ret += ",\n";
		}
		ret += Registry::serialize(*it);
		++it;
	}
	ret += "]";
	return ret;
}

vector<Registry> Registry::unserialize(const string& str) {
	vector<Registry> v;
	Document d;
	try {
		d.Parse<0>(str.c_str());
		if (d.IsArray()) {
			for (int i = 0; i < d.Size(); i++) {

				if (d[i].HasMember("name") && d[i].HasMember("host") && d[i].HasMember("port")) {
					Registry reg;
					reg.name = d[i]["name"].GetString();
					reg.host = d[i]["host"].GetString();
					if (d[i]["port"].IsInt() == true) {
						reg.port = d[i]["port"].GetInt();
					} else if (d[i]["port"].IsString() == true) {
						string s = d[i]["port"].GetString();
						reg.port = (int) atol(s.c_str());
					} else
						continue;
					reg.ephemeral = "";
					reg.ctime = utils::now_in_ms();
					v.push_back(reg);
				}

			}
		}
	} catch (...) {
		logger::error("fail to unserialize Registry %s", str.c_str());
		cout << "fail to unserialize Registry xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" << str << endl;
	}
	return v;
}

} /* namespace FinagleRegistryProxy */

//global method
bool operator==(const FinagleRegistryProxy::Registry& l, const FinagleRegistryProxy::Registry& r) {
	return &l == &r ? true : l.name == r.name && l.host == r.host && l.port == r.port;
}
//global method
bool operator==(FinagleRegistryProxy::Registry& l, FinagleRegistryProxy::Registry& r) {
	return &l == &r ? true : l.name == r.name && l.host == r.host && l.port == r.port;
}
