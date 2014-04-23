/**
 * Autogenerated by Thrift
 *
 * DO NOT EDIT UNLESS YOU ARE SURE THAT YOU KNOW WHAT YOU ARE DOING
 */
#ifndef scribe_H
#define scribe_H

#include <TProcessor.h>
#include "scribe_types.h"
#include "FacebookService.h"

namespace scribe { namespace thrift {

class scribeIf : virtual public facebook::fb303::FacebookServiceIf {
 public:
  virtual ~scribeIf() {}
  virtual ResultCode::type Log(const std::vector<LogEntry> & messages) = 0;
};

class scribeNull : virtual public scribeIf , virtual public facebook::fb303::FacebookServiceNull {
 public:
  virtual ~scribeNull() {}
  ResultCode::type Log(const std::vector<LogEntry> & /* messages */) {
    ResultCode::type _return = (ResultCode::type)0;
    return _return;
  }
};

typedef struct _scribe_Log_args__isset {
  _scribe_Log_args__isset() : messages(false) {}
  bool messages;
} _scribe_Log_args__isset;

class scribe_Log_args {
 public:

  scribe_Log_args() {
  }

  virtual ~scribe_Log_args() throw() {}

  std::vector<LogEntry>  messages;

  _scribe_Log_args__isset __isset;

  bool operator == (const scribe_Log_args & rhs) const
  {
    if (!(messages == rhs.messages))
      return false;
    return true;
  }
  bool operator != (const scribe_Log_args &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const scribe_Log_args & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};


class scribe_Log_pargs {
 public:


  virtual ~scribe_Log_pargs() throw() {}

  const std::vector<LogEntry> * messages;

  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};

typedef struct _scribe_Log_result__isset {
  _scribe_Log_result__isset() : success(false) {}
  bool success;
} _scribe_Log_result__isset;

class scribe_Log_result {
 public:

  scribe_Log_result() {
  }

  virtual ~scribe_Log_result() throw() {}

  ResultCode::type success;

  _scribe_Log_result__isset __isset;

  bool operator == (const scribe_Log_result & rhs) const
  {
    if (!(success == rhs.success))
      return false;
    return true;
  }
  bool operator != (const scribe_Log_result &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const scribe_Log_result & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};

typedef struct _scribe_Log_presult__isset {
  _scribe_Log_presult__isset() : success(false) {}
  bool success;
} _scribe_Log_presult__isset;

class scribe_Log_presult {
 public:


  virtual ~scribe_Log_presult() throw() {}

  ResultCode::type* success;

  _scribe_Log_presult__isset __isset;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);

};

class scribeClient : virtual public scribeIf, public facebook::fb303::FacebookServiceClient {
 public:
  scribeClient(boost::shared_ptr< ::apache::thrift::protocol::TProtocol> prot) :
    facebook::fb303::FacebookServiceClient(prot, prot) {}
  scribeClient(boost::shared_ptr< ::apache::thrift::protocol::TProtocol> iprot, boost::shared_ptr< ::apache::thrift::protocol::TProtocol> oprot) :
    facebook::fb303::FacebookServiceClient(iprot, oprot) {}
  boost::shared_ptr< ::apache::thrift::protocol::TProtocol> getInputProtocol() {
    return piprot_;
  }
  boost::shared_ptr< ::apache::thrift::protocol::TProtocol> getOutputProtocol() {
    return poprot_;
  }
  ResultCode::type Log(const std::vector<LogEntry> & messages);
  void send_Log(const std::vector<LogEntry> & messages);
  ResultCode::type recv_Log();
};

class scribeProcessor : virtual public ::apache::thrift::TProcessor, public facebook::fb303::FacebookServiceProcessor {
 protected:
  boost::shared_ptr<scribeIf> iface_;
  virtual bool process_fn(::apache::thrift::protocol::TProtocol* iprot, ::apache::thrift::protocol::TProtocol* oprot, std::string& fname, int32_t seqid);
 private:
  std::map<std::string, void (scribeProcessor::*)(int32_t, ::apache::thrift::protocol::TProtocol*, ::apache::thrift::protocol::TProtocol*)> processMap_;
  void process_Log(int32_t seqid, ::apache::thrift::protocol::TProtocol* iprot, ::apache::thrift::protocol::TProtocol* oprot);
 public:
  scribeProcessor(boost::shared_ptr<scribeIf> iface) :
    facebook::fb303::FacebookServiceProcessor(iface),
    iface_(iface) {
    processMap_["Log"] = &scribeProcessor::process_Log;
  }

  virtual bool process(boost::shared_ptr< ::apache::thrift::protocol::TProtocol> piprot, boost::shared_ptr< ::apache::thrift::protocol::TProtocol> poprot);
  virtual ~scribeProcessor() {}
};

class scribeMultiface : virtual public scribeIf, public facebook::fb303::FacebookServiceMultiface {
 public:
  scribeMultiface(std::vector<boost::shared_ptr<scribeIf> >& ifaces) : ifaces_(ifaces) {
    std::vector<boost::shared_ptr<scribeIf> >::iterator iter;
    for (iter = ifaces.begin(); iter != ifaces.end(); ++iter) {
      facebook::fb303::FacebookServiceMultiface::add(*iter);
    }
  }
  virtual ~scribeMultiface() {}
 protected:
  std::vector<boost::shared_ptr<scribeIf> > ifaces_;
  scribeMultiface() {}
  void add(boost::shared_ptr<scribeIf> iface) {
    facebook::fb303::FacebookServiceMultiface::add(iface);
    ifaces_.push_back(iface);
  }
 public:
  ResultCode::type Log(const std::vector<LogEntry> & messages) {
    uint32_t sz = ifaces_.size();
    for (uint32_t i = 0; i < sz; ++i) {
      if (i == sz - 1) {
        return ifaces_[i]->Log(messages);
      } else {
        ifaces_[i]->Log(messages);
      }
    }
  }

};

}} // namespace

#endif
