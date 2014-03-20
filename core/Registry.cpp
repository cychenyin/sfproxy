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
}

Registry::~Registry() {
	this->port = 0;
}

} /* namespace FinagleRegistryProxy */
