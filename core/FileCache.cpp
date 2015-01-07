/*
 * FileCache.cpp
 *
 *  Created on: Dec 25, 2014
 *      Author: asdf
 */

#include "FileCache.h"
#include "limits.h"
#include "../log/logger.h"

#define INITIAL_BUFFER_SIZE (1024)
#define LARGE_BUFFER_SIZE (16 * INITIAL_BUFFER_SIZE) /* arbitrarily chosen */
#define UINT_SIZE 4

namespace FinagleRegistryProxy {

FileCache::FileCache(const std::string& name) :
		filename(name), inputBuffer(0), bufferSize(0) {
}

FileCache::~FileCache() {
	if (inputBuffer) {
		delete[] inputBuffer;
		inputBuffer = NULL;
	}
}

bool FileCache::open_read() {
	return open(fstream::in);
}

bool FileCache::open_write() {
	// open file for write in append mode
	// ios_base::openmode mode = fstream::out | fstream::app;
	ios_base::openmode mode = fstream::out;
	return open(mode);
}

bool FileCache::open(ios_base::openmode mode) {

	if (file.is_open()) {
		return false;
	}

	file.open(filename.c_str(), mode);

	return file.good();
}

bool FileCache::is_open() {
	return file.is_open();
}

void FileCache::close() {
	if (file.is_open()) {
		file.close();
	}
}
bool FileCache::write(const std::string& data) {

	if (!file.is_open()) {
		return false;
	}

	file << data;
	if (file.bad()) {
		return false;
	}
	return true;
}

void FileCache::flush() {
	if (file.is_open()) {
		file.flush();
	}
}

long FileCache::read_all(std::string& _return) {
	file >> _return;
	return _return.size();

//	assert(file);
//	unsigned long size = 0;
//	mutex.lock();
//	try {
//		file.seekg(0, file.end);
//		size = file.tellg();
//		file.seekg(0, file.beg);
//
//		if (inputBuffer) {
//			delete[] inputBuffer;
//			inputBuffer = NULL;
//		}
//		inputBuffer = new char[size];
//
//		file.read(inputBuffer, size);
//		if (!file.good()) {
//			logger::warn("Fail to read_all of file %s", filename.c_str());
//		} else {
//			_return.assign(inputBuffer, size);
//		}
//		delete[] inputBuffer;
//		inputBuffer = NULL;
//	} catch (const exception& e) {
//		logger::warn("Fail to read_all of file %s cause of exception", filename.c_str());
//	}
//	mutex.unlock();

//	return size;
}

unsigned long FileCache::file_size() {
	unsigned long size = 0;
	try {
		size = boost::filesystem::file_size(filename.c_str());
	} catch (const std::exception& e) {
		logger::warn("Failed to get size for file <%s> error <%s>", filename.c_str(), e.what());
		size = 0;
	}
	return size;
}
bool FileCache::create_directory(std::string path) {
	try {
		boost::filesystem::create_directories(path);
	} catch (const std::exception& e) {
		logger::warn("Exception < %s > in FileCache::createDirectory for path %s ", e.what(), path.c_str());
		return false;
	}

	return true;
}

} /* namespace FinagleRegistryProxy */
