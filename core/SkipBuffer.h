/*
 * SkipBuffer.h
 *
 *  Created on: May 23, 2014
 *      Author: asdf
 */

#ifndef SKIPBUFFER_H_
#define SKIPBUFFER_H_

#include <concurrency/Mutex.h>
#include <unistd.h>
#include <iterator>
#include <map>
#include <string>
#include <utility>
#include <vector>

#include "../frproxy.h"

using namespace std;
using namespace apache::thrift::concurrency;
using boost::shared_ptr;

namespace FinagleRegistryProxy {
// safe to insert & erase; key is zkpath, value is timestamp in in ms
class SkipBuffer: public map<string, uint32_t> {
private:
	Mutex mutex_;
	uint32_t timeout_;
public:
	SkipBuffer(uint32_t timeout) :
			timeout_(timeout) {
	}
	~SkipBuffer() {
	}

	std::pair<iterator, bool> insert(string zkpath) {
		uint32_t now = utils::now_in_ms();
		mutex_.lock();
		map<string, uint32_t>::iterator it = this->find(zkpath);
		if (it != this->end() && now - (*it).second > timeout_) {
			map<string, uint32_t>::erase(it);
		}
		std::pair<iterator, bool> ret = map<string, uint32_t>::insert(pair<string, uint32_t>(zkpath, now));
		mutex_.unlock();
		return ret;
	}

	void erase(string key) {
		mutex_.lock();
		map<string, uint32_t>::iterator __position = this->find(key);
		if (__position != this->end()) {
			map<string, uint32_t>::erase(__position);
		}
		mutex_.unlock();
	}

	string dump() {
		stringstream ss;
		uint32_t now = utils::now_in_ms();
		mutex_.lock();
		ss << "skip buffer:" << "---------------------------------------------" << endl;
		ss << "	size: " << size() << endl;
		map<string, uint32_t>::iterator it = this->begin();
		while(it != this->end()) {
			ss << "	" << it->first << "	ms elapse" << (now - it->second) << endl;
			it ++;
		}
		mutex_.unlock();
		return ss.str();
	}

};
// SkipBuffer


} // namespace FinagleRegistryProxy

#endif /* SKIPBUFFER_H_ */
