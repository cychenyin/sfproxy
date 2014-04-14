/*
 * Registry.cpp
 *
 *  Created on: Mar 17, 2014
 *      Author: asdf
 */

#include "Registry.h"
#include <sstream>
#include <boost/format.hpp>

namespace FinagleRegistryProxy {

Registry::Registry() {
	this->name = "";
	this->host = "";
	this->port = 0;
	ctime = 0;
}

Registry::Registry(string name, string host, int port) {
	this->name = name;
	this->host = host;
	this->port = port;
	ctime = 0;
}
Registry::~Registry() {
	this->port = 0;
}

bool Registry::check() {
	return !name.empty() && !host.empty() && port > 0;
}

int Registry::weight() {
	long elapse = time(0) - ctime;
	return elapse < 3600 && elapse > 0 ? elapse : 3600;
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
string Registry::to_json_string(Registry& r) {
	// string ret = "{";
	stringstream ss;
	ss << "{\"name\":\"" << r.name;
	ss << "\",\"host\":\"" << r.host;
	ss << "\",\"port\":" << r.port;
	ss << ",\"e\"=\"" << r.ephemeral;
	ss << "\",\"weight\":" << r.weight();
	ss << " }";
	return ss.str();
}

// 80-100us
// use stringstream, perfermance import 100% than using rapidJson
string Registry::to_json_string2(Registry& r) {

	stringstream s;
	s << r.port;
	stringstream w;
	w << r.weight();

	string ss;
	ss += "{";
	ss += "\"name\":\"";
	ss += r.name;
	ss += "\",\"host\":\"";
	ss += r.host;
	ss += "\",\"port\":";
	ss += s.str();
	ss += ",\"e\":\"";
	ss += r.ephemeral;
	ss += "\",\"weight\":";
	ss += w.str();
	ss += "}";
	return ss;
}


// 120us
string Registry::to_json_string3(Registry& r) {
	Document document;
	Document::AllocatorType& allocator = document.GetAllocator();

//    Value contact(kArrayType);
//    Value contact2(kArrayType);
//    Value root(kArrayType);
//    contact.PushBack("Lu//a\"", allocator).PushBack("Mio", allocator).PushBack("", allocator);
//    contact2.PushBack("Lu// a", allocator).PushBack("Mio", allocator).PushBack("", allocator);
//    root.PushBack(contact, allocator);
//    root.PushBack(contact2, allocator);

	Value root(kObjectType);
	{
		Value jname(r.name.c_str());
		Value jhost(r.host.c_str());
		Value je(r.ephemeral.c_str());
		Value jport(r.port);
		Value jweight(r.weight());
		root.AddMember("name", jname, allocator);
		root.AddMember("host", jhost, allocator);
		root.AddMember("port", jport, allocator);
		root.AddMember("e", je, allocator);
		root.AddMember("weight", jweight, allocator);
	}

	StringBuffer buffer;
	Writer<StringBuffer> writer(buffer);
	root.Accept(writer);
	return buffer.GetString();
}

//210us
string Registry::to_json_string4(Registry& r) {
	boost::format fmt("{\"name\"=\"%s\",\"host\"=\"%s\",\"port\"=%i,\"e\"=\"%s\",\"weight\"=%i}");
	fmt % r.name % r.host % r.port % r.ephemeral % r.weight();
	return fmt.str();
}

string Registry::to_json_string(vector<Registry> v) {
	string ret;
	ret += "[";
	vector<Registry>::iterator it = v.begin();
	while (it != v.end()) {
		if (ret.size() > 2) {
			ret += ",";
		}
		ret += Registry::to_json_string(*it);
		++it;
	}
	ret += "]";
	return ret;
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
