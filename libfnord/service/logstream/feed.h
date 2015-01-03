/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORD_LOGSTREAM_SERVICE_FEED_H
#define _FNORD_LOGSTREAM_SERVICE_FEED_H
#include <deque>
#include "fnord/comm/feed.h"
#include "fnord/comm/rpc.h"
#include "fnord/service/logstream/logstreamentry.h"

namespace fnord {
namespace logstream_service {

class LogStreamServiceFeed : public fnord::comm::Feed {
public:
  static const int kDefaultBatchSize = 1024;
  static const int kDefaultBufferSize = 8192;

  LogStreamServiceFeed(
      const std::string& name,
      fnord::comm::RPCChannel* rpc_channel,
      int batch_size = kDefaultBatchSize,
      int buffer_size = kDefaultBufferSize);

  std::string offset() const override;

  void append(const std::string& entry) override;
  bool getNextEntry(std::string* entry) override;

  void setOption(const std::string& optname,const std::string& optval) override;

protected:
  void maybeFillBuffer();
  void fillBuffer();
  void insertDone();

  fnord::comm::RPCChannel* rpc_channel_;
  int batch_size_;
  int buffer_size_;
  uint64_t offset_;

  std::mutex fetch_mutex_;
  std::deque<LogStreamEntry> fetch_buf_;
  std::unique_ptr<
      comm::RPC<
          std::vector<LogStreamEntry>,
          std::tuple<std::string, uint64_t, int>>> cur_fetch_rpc_;

  std::mutex insert_mutex_;
  std::deque<std::string> insert_buf_;
  std::unique_ptr<
      comm::RPC<
          uint64_t,
          std::tuple<std::string, std::string>>> cur_insert_rpc_;
};

}
}
#endif