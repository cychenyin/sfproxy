/**
 * Autogenerated by Thrift Compiler (0.9.1)
 *
 * DO NOT EDIT UNLESS YOU ARE SURE THAT YOU KNOW WHAT YOU ARE DOING
 *  @generated
 */
#ifndef RegistryProxy_H
#define RegistryProxy_H

#include <thrift/TDispatchProcessor.h>
#include "proxy_types.h"

namespace FinagleRegistryProxy {

class RegistryProxyIf {
 public:
  virtual ~RegistryProxyIf() {}
  virtual void get(std::string& _return, const std::string& serviceName) = 0;
  virtual void remove(std::string& _return, const std::string& serviceName, const std::string& host, const int32_t port) = 0;
};

class RegistryProxyIfFactory {
 public:
  typedef RegistryProxyIf Handler;

  virtual ~RegistryProxyIfFactory() {}

  virtual RegistryProxyIf* getHandler(const ::apache::thrift::TConnectionInfo& connInfo) = 0;
  virtual void releaseHandler(RegistryProxyIf* /* handler */) = 0;
};

class RegistryProxyIfSingletonFactory : virtual public RegistryProxyIfFactory {
 public:
  RegistryProxyIfSingletonFactory(const boost::shared_ptr<RegistryProxyIf>& iface) : iface_(iface) {}
  virtual ~RegistryProxyIfSingletonFactory() {}

  virtual RegistryProxyIf* getHandler(const ::apache::thrift::TConnectionInfo&) {
    return iface_.get();
  }
  virtual void releaseHandler(RegistryProxyIf* /* handler */) {}

 protected:
  boost::shared_ptr<RegistryProxyIf> iface_;
};

class RegistryProxyNull : virtual public RegistryProxyIf {
 public:
  virtual ~RegistryProxyNull() {}
  void get(std::string& /* _return */, const std::string& /* serviceName */) {
    return;
  }
  void remove(std::string& /* _return */, const std::string& /* serviceName */, const std::string& /* host */, const int32_t /* port */) {
    return;
  }
};


class RegistryProxy_get_args {
 public:

  RegistryProxy_get_args() : serviceName() {
  }

  virtual ~RegistryProxy_get_args() throw() {}

  std::string serviceName;

  void __set_serviceName(const std::string& val) {
    serviceName = val;
  }

  bool operator == (const RegistryProxy_get_args & rhs) const
  {
    if (!(serviceName == rhs.serviceName))
      return false;
    return true;
  }
  bool operator != (const RegistryProxy_get_args &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const RegistryProxy_get_args & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};


class RegistryProxy_get_pargs {
 public:


  virtual ~RegistryProxy_get_pargs() throw() {}

  const std::string* serviceName;

  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};

typedef struct _RegistryProxy_get_result__isset {
  _RegistryProxy_get_result__isset() : success(false) {}
  bool success;
} _RegistryProxy_get_result__isset;

class RegistryProxy_get_result {
 public:

  RegistryProxy_get_result() : success() {
  }

  virtual ~RegistryProxy_get_result() throw() {}

  std::string success;

  _RegistryProxy_get_result__isset __isset;

  void __set_success(const std::string& val) {
    success = val;
  }

  bool operator == (const RegistryProxy_get_result & rhs) const
  {
    if (!(success == rhs.success))
      return false;
    return true;
  }
  bool operator != (const RegistryProxy_get_result &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const RegistryProxy_get_result & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};

typedef struct _RegistryProxy_get_presult__isset {
  _RegistryProxy_get_presult__isset() : success(false) {}
  bool success;
} _RegistryProxy_get_presult__isset;

class RegistryProxy_get_presult {
 public:


  virtual ~RegistryProxy_get_presult() throw() {}

  std::string* success;

  _RegistryProxy_get_presult__isset __isset;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);

};

typedef struct _RegistryProxy_remove_args__isset {
  _RegistryProxy_remove_args__isset() : host(false), port(false) {}
  bool host;
  bool port;
} _RegistryProxy_remove_args__isset;

class RegistryProxy_remove_args {
 public:

  RegistryProxy_remove_args() : serviceName(), host(), port(0) {
  }

  virtual ~RegistryProxy_remove_args() throw() {}

  std::string serviceName;
  std::string host;
  int32_t port;

  _RegistryProxy_remove_args__isset __isset;

  void __set_serviceName(const std::string& val) {
    serviceName = val;
  }

  void __set_host(const std::string& val) {
    host = val;
  }

  void __set_port(const int32_t val) {
    port = val;
  }

  bool operator == (const RegistryProxy_remove_args & rhs) const
  {
    if (!(serviceName == rhs.serviceName))
      return false;
    if (!(host == rhs.host))
      return false;
    if (!(port == rhs.port))
      return false;
    return true;
  }
  bool operator != (const RegistryProxy_remove_args &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const RegistryProxy_remove_args & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};


class RegistryProxy_remove_pargs {
 public:


  virtual ~RegistryProxy_remove_pargs() throw() {}

  const std::string* serviceName;
  const std::string* host;
  const int32_t* port;

  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};

typedef struct _RegistryProxy_remove_result__isset {
  _RegistryProxy_remove_result__isset() : success(false) {}
  bool success;
} _RegistryProxy_remove_result__isset;

class RegistryProxy_remove_result {
 public:

  RegistryProxy_remove_result() : success() {
  }

  virtual ~RegistryProxy_remove_result() throw() {}

  std::string success;

  _RegistryProxy_remove_result__isset __isset;

  void __set_success(const std::string& val) {
    success = val;
  }

  bool operator == (const RegistryProxy_remove_result & rhs) const
  {
    if (!(success == rhs.success))
      return false;
    return true;
  }
  bool operator != (const RegistryProxy_remove_result &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const RegistryProxy_remove_result & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};

typedef struct _RegistryProxy_remove_presult__isset {
  _RegistryProxy_remove_presult__isset() : success(false) {}
  bool success;
} _RegistryProxy_remove_presult__isset;

class RegistryProxy_remove_presult {
 public:


  virtual ~RegistryProxy_remove_presult() throw() {}

  std::string* success;

  _RegistryProxy_remove_presult__isset __isset;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);

};

class RegistryProxyClient : virtual public RegistryProxyIf {
 public:
  RegistryProxyClient(boost::shared_ptr< ::apache::thrift::protocol::TProtocol> prot) :
    piprot_(prot),
    poprot_(prot) {
    iprot_ = prot.get();
    oprot_ = prot.get();
  }
  RegistryProxyClient(boost::shared_ptr< ::apache::thrift::protocol::TProtocol> iprot, boost::shared_ptr< ::apache::thrift::protocol::TProtocol> oprot) :
    piprot_(iprot),
    poprot_(oprot) {
    iprot_ = iprot.get();
    oprot_ = oprot.get();
  }
  boost::shared_ptr< ::apache::thrift::protocol::TProtocol> getInputProtocol() {
    return piprot_;
  }
  boost::shared_ptr< ::apache::thrift::protocol::TProtocol> getOutputProtocol() {
    return poprot_;
  }
  void get(std::string& _return, const std::string& serviceName);
  void send_get(const std::string& serviceName);
  void recv_get(std::string& _return);
  void remove(std::string& _return, const std::string& serviceName, const std::string& host, const int32_t port);
  void send_remove(const std::string& serviceName, const std::string& host, const int32_t port);
  void recv_remove(std::string& _return);
 protected:
  boost::shared_ptr< ::apache::thrift::protocol::TProtocol> piprot_;
  boost::shared_ptr< ::apache::thrift::protocol::TProtocol> poprot_;
  ::apache::thrift::protocol::TProtocol* iprot_;
  ::apache::thrift::protocol::TProtocol* oprot_;
};

class RegistryProxyProcessor : public ::apache::thrift::TDispatchProcessor {
 protected:
  boost::shared_ptr<RegistryProxyIf> iface_;
  virtual bool dispatchCall(::apache::thrift::protocol::TProtocol* iprot, ::apache::thrift::protocol::TProtocol* oprot, const std::string& fname, int32_t seqid, void* callContext);
 private:
  typedef  void (RegistryProxyProcessor::*ProcessFunction)(int32_t, ::apache::thrift::protocol::TProtocol*, ::apache::thrift::protocol::TProtocol*, void*);
  typedef std::map<std::string, ProcessFunction> ProcessMap;
  ProcessMap processMap_;
  void process_get(int32_t seqid, ::apache::thrift::protocol::TProtocol* iprot, ::apache::thrift::protocol::TProtocol* oprot, void* callContext);
  void process_remove(int32_t seqid, ::apache::thrift::protocol::TProtocol* iprot, ::apache::thrift::protocol::TProtocol* oprot, void* callContext);
 public:
  RegistryProxyProcessor(boost::shared_ptr<RegistryProxyIf> iface) :
    iface_(iface) {
    processMap_["get"] = &RegistryProxyProcessor::process_get;
    processMap_["remove"] = &RegistryProxyProcessor::process_remove;
  }

  virtual ~RegistryProxyProcessor() {}
};

class RegistryProxyProcessorFactory : public ::apache::thrift::TProcessorFactory {
 public:
  RegistryProxyProcessorFactory(const ::boost::shared_ptr< RegistryProxyIfFactory >& handlerFactory) :
      handlerFactory_(handlerFactory) {}

  ::boost::shared_ptr< ::apache::thrift::TProcessor > getProcessor(const ::apache::thrift::TConnectionInfo& connInfo);

 protected:
  ::boost::shared_ptr< RegistryProxyIfFactory > handlerFactory_;
};

class RegistryProxyMultiface : virtual public RegistryProxyIf {
 public:
  RegistryProxyMultiface(std::vector<boost::shared_ptr<RegistryProxyIf> >& ifaces) : ifaces_(ifaces) {
  }
  virtual ~RegistryProxyMultiface() {}
 protected:
  std::vector<boost::shared_ptr<RegistryProxyIf> > ifaces_;
  RegistryProxyMultiface() {}
  void add(boost::shared_ptr<RegistryProxyIf> iface) {
    ifaces_.push_back(iface);
  }
 public:
  void get(std::string& _return, const std::string& serviceName) {
    size_t sz = ifaces_.size();
    size_t i = 0;
    for (; i < (sz - 1); ++i) {
      ifaces_[i]->get(_return, serviceName);
    }
    ifaces_[i]->get(_return, serviceName);
    return;
  }

  void remove(std::string& _return, const std::string& serviceName, const std::string& host, const int32_t port) {
    size_t sz = ifaces_.size();
    size_t i = 0;
    for (; i < (sz - 1); ++i) {
      ifaces_[i]->remove(_return, serviceName, host, port);
    }
    ifaces_[i]->remove(_return, serviceName, host, port);
    return;
  }

};

} // namespace

#endif
