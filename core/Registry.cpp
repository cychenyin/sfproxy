/*
 * Registry.cpp
 *
 *  Created on: Mar 17, 2014
 *      Author: asdf
 */

#include "Registry.h"

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
	int elapse = time(0) / 1000 - ctime;
	return elapse < 3600 ? elapse : 3600;
}

bool Registry::equals(Registry& r) {
	return this == &r ? true : name == r.name && host == r.host && port == r.port;
}
bool Registry::operator==(Registry& r) {
	return this == &r ? true : name == r.name && host == r.host && port == r.port;
}

string Registry::toJsonString(Registry& r) {
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
		Value jhost(r.name.c_str());
		Value jport(r.name.c_str());
		Value jweight(r.name.c_str());

		root.AddMember("name", jname, allocator);
		root.AddMember("host", jhost, allocator);
		root.AddMember("port", jport, allocator);
		root.AddMember("weight", jweight, allocator);
	}

	StringBuffer buffer;
	Writer<StringBuffer> writer(buffer);
	root.Accept(writer);
	return buffer.GetString();
}

string Registry::toJsonStringx(vector<Registry> v) {
//static string Registry::toJsonString(vector<Registry> &v) {
	string ret;
	vector<Registry>::iterator it = v.begin();
	while (it != v.end()) {
		ret += Registry::toJsonString(*it);
	}
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
