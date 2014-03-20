/*
 * Registry.h
 *
 *  Created on: Mar 17, 2014
 *      Author: asdf
 */

#ifndef REGISTRY_H_
#define REGISTRY_H_

#include <string>

namespace FinagleRegistryProxy {

class Registry {
public:
	std::string name;
	std::string host;
	int port;
	int weight;
	Registry();
	virtual ~Registry();

	std::string json;

	Registry fromJsonString();
};

} /* namespace FinagleRegistryProxy */

#endif /* REGISTRY_H_ */
