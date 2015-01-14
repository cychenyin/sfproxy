/*
 * Polymorphism.cpp
 *
 *  Created on: Jan 14, 2015
 *      Author: asdf
 */

#define BOOST_TEST_NO_MAIN

#include <iostream>
#include <sstream>
#include <stdio.h>
#include <boost/test/unit_test.hpp>

#include <concurrency/Exception.h>
#include <concurrency/PosixThreadFactory.h>
#include <concurrency/Thread.h>
#include <concurrency/ThreadManager.h>

BOOST_AUTO_TEST_SUITE( smart_point_fixture_suite )

using namespace std;
using boost::shared_ptr;
using namespace apache::thrift::concurrency;

class ServerHandler {

};

class ScheduledTask: virtual public Runnable {
protected:
	ServerHandler* handler;
public:
	ScheduledTask(ServerHandler* _handler) :
			handler(_handler) {
	}
	virtual ~ScheduledTask() {
		handler = NULL;
	}
	void run() {
		cout << "ScheduledTask run, middle class" << endl;
	}
};
// class TaskScheduler

class ReloadTask: public ScheduledTask {
public:
	ReloadTask(ServerHandler* _handler) :
			ScheduledTask(_handler) {
	}
	virtual ~ReloadTask() {
	}
	void run() {
		cout << "reload run" << endl;
	}
};
// class ReloadTask

class TaskInfo {
public:
	string name;
	shared_ptr<ScheduledTask> runner;
	int interval_in_ms;
	// last run time; accurate to ms,
	int last_run_ms;

public:
	TaskInfo(ScheduledTask* _runner, int _interval_in_ms) :
			interval_in_ms(_interval_in_ms), last_run_ms(0) {
		name = "";
		runner = shared_ptr<ScheduledTask>(_runner);
	}
	virtual ~TaskInfo() {
//		if (runner) {
//			cout << name << " delete runner in TaskInfo" << endl;
//			delete runner;
//		}
	}
};
// class TaskInfo

class F {
public:
	F() {
		BOOST_TEST_MESSAGE("----------------------------------------------");
	}
	~F() {
	}
};

BOOST_FIXTURE_TEST_CASE( empty, F) {
	BOOST_CHECK(true);
}

BOOST_FIXTURE_TEST_CASE( init, F) {
	Runnable *pr;
	ServerHandler *pserver = new ServerHandler();
	ReloadTask *ptask = new ReloadTask(pserver);

	pr = ptask;
	pr->run();
	ptask->run();

	delete pserver;
	pserver = NULL;
	BOOST_CHECK(true);
}

BOOST_FIXTURE_TEST_CASE(run_by_taskinfo, F) {
	ServerHandler *pserver = new ServerHandler();
	ReloadTask *ptask = new ReloadTask(pserver);
	TaskInfo *pinfo = new TaskInfo(ptask, 1);

	pinfo->runner->run();

	delete pinfo;
	pinfo = NULL;
	delete pserver;
	pserver = NULL;

	BOOST_CHECK(true);
}

BOOST_FIXTURE_TEST_CASE(run_by_point, F) {
	ServerHandler *pserver = new ServerHandler();
	ReloadTask *ptask = new ReloadTask(pserver);
	TaskInfo *pinfo = new TaskInfo(ptask, 1);

	Runnable *pr = pinfo->runner.get();
	pr->run();

	ScheduledTask *pst = pinfo->runner.get();
	pst->run();

	delete pinfo;
	delete pserver;

	BOOST_CHECK(true);
}

BOOST_FIXTURE_TEST_CASE(run_by_smartpoint, F) {
	ServerHandler *pserver = new ServerHandler();
	ReloadTask *ptask = new ReloadTask(pserver);
	TaskInfo *pinfo = new TaskInfo(ptask, 100);

	shared_ptr<Runnable> pr(pinfo->runner.get());
	pr->run();

//	{
//		shared_ptr<Runnable> pr(pinfo->runner.get());
//		pr->run();
//	}

//	{
//		shared_ptr<Runnable> pr(pinfo->runner.get());
//		pr->run();
//	}

	delete pserver;

	BOOST_CHECK(true);
}

BOOST_FIXTURE_TEST_CASE(run_by_smartpoint_2, F) {
	// threadManager->add(shared_ptr<ScheduledTask>(task->runner), async_wait_timeout);

	shared_ptr<ThreadManager> threadManager = ThreadManager::newSimpleThreadManager(3, 100);
	shared_ptr<PosixThreadFactory> threadFactory = shared_ptr<PosixThreadFactory>(new PosixThreadFactory());
	threadManager->threadFactory(threadFactory);
	threadManager->start();

	ServerHandler *pserver = new ServerHandler();
	ReloadTask *ptask = new ReloadTask(pserver);
	TaskInfo *pinfo = new TaskInfo(ptask, 100);

//	shared_ptr<Runnable> pr(pinfo->runner);
//	threadManager->add(pr, 100, 100);
//	sleep(1);
//	cout << "-- step 1 end" << endl;
//
//	for (int i = 0; i < 3; i++) {
//		threadManager->add(pr, 100, 100);
//		usleep(10);
//	}
//	sleep(1);
//	cout << "-- step 2 end" << endl;


	shared_ptr<Runnable> task(pinfo->runner);
	for (int i = 0; i < 3; i++) {
		threadManager->add(task, 100, 100);
		cout << "thread committed " << endl;
		usleep(100);
	}
	sleep(3);
	cout << "-- step 3 end" << endl;

	cout << "main thread sleep end " << endl;
	threadManager->stop();
	cout << "-- step 4 end" << endl;
	BOOST_CHECK(true);
}

BOOST_AUTO_TEST_SUITE_END()
