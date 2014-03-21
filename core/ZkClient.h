/*
 * ZkClient.h
 *
 *  Created on: Mar 18, 2014
 *      Author: asdf
 *
 * zk api:
 //create a handle to used communicate with zookeeper
zhandle_t *zookeeper_init(const char *host, watcher_fn fn, int recv_timeout, const clientid_t *clientid, void *context, int flags)
//create a node synchronously
int zoo_create(zhandle_t *zh, const char *path, const char *value,int valuelen,
   const struct ACL_vector *acl, int flags,char *path_buffer, int path_buffer_len);
//lists the children of a node synchronously.
int zoo_wget_children(zhandle_t *zh, const char *path, watcher_fn watcher, void* watcherCtx, struct String_vector *strings)
//close the zookeeper handle and free up any resources.
int zookeeper_close(zhandle_t *zh)
// ZOOAPI int zoo_wget(zhandle_t *zh, const char *path,watcher_fn watcher, void* watcherCtx, char *buffer, int* buffer_len, struct Stat *stat);
 */

#ifndef ZKCLIENT_H_
#define ZKCLIENT_H_

#include <iostream>
#include <string.h>
#include <errno.h>

#include <zookeeper/zookeeper.h>
#include <zookeeper/zk_adaptor.h>
#include <zookeeper/zookeeper_log.h>


#include "JsonUtil.h"
#include "RegistryCache.h"

using namespace std;

namespace FinagleRegistryProxy {

const int ZK_MAX_CONNECT_RETRY_TIMES = 10;

class ZkClient {
public:
	// hosts format: 127.0.0.1:2181,127.0.0.1:2182
	ZkClient(string hosts, RegistryCache *pcache);

	virtual ~ZkClient();
	void ConnectZK();
	void UpdateServices(string serviceName);
	void UpdateService(string serviceName, string subNodeName);
	static void ChildrenWatcher(zhandle_t *zh, int type, int state, const char *path, void *watcherCtx);
	static void GetWatcher(zhandle_t *zh, int type, int state, const char *path, void *watcherCtx);
	static void GlobalWatcher(zhandle_t *zh, int type, int state, const char *path, void *watcherCtx);
	void Close();
//	void InitWatcher(zhandle_t *zh, int type, int state, const char *path, void *watcher_ctx);
	void DumpStat(struct Stat *stat);
	void parse(string json);

private:
	RegistryCache *pcache;
	zhandle_t *zhandle_;
	string zk_hosts_;
	int timeout_;
	void Init();
};

class ZkClientContext {
public:
	ZkClient *client;
	string serviceName;

	ZkClientContext();
	ZkClientContext(ZkClient *client, string serviceName);
	virtual ~ZkClientContext();
};

} /* namespace FinagleRegistryProxy */

#endif /* ZKCLIENT_H_ */
