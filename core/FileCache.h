/*
 * FileCache.h
 *
 *  Created on: Dec 25, 2014
 *      Author: asdf
 */

#ifndef FILECACHE_H_
#define FILECACHE_H_

#include <sstream>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <boost/version.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/convenience.hpp>
#include "concurrency/Mutex.h"

namespace FinagleRegistryProxy {

using namespace std;

class FileCache {
public:
	FileCache(const std::string& name);
	virtual ~FileCache();

	bool open_read();
	bool open_write();
	bool is_open();
	void close();
	bool write(const std::string& data);
	// used for small file read
	long read_all(std::string& _return);
	void flush();
	unsigned long file_size();
	bool create_directory(std::string path);
protected:
	string filename;

private:
	bool open(std::ios_base::openmode mode);
	apache::thrift::concurrency::Mutex mutex;

	std::fstream file;
	char* inputBuffer;
	unsigned bufferSize;

	// disallow copy, assignment, and empty construction
	FileCache();
	FileCache(FileCache& fc);
	FileCache& operator=(FileCache& fc);
};

} /* namespace FinagleRegistryProxy */

#endif /* FILECACHE_H_ */
