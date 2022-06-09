#pragma once

#include <hiredis/hiredis.h>
#include <wangle/codec/ByteToMessageDecoder.h>
#include <wangle/codec/MessageToByteEncoder.h>

using namespace wangle;
using namespace folly;

class Resp {
 public:
  Resp(redisReply* reply) : reply_(reply) {}
  ~Resp() {
    if (reply_) {
      freeReplyObject(reply_);
    }
  }
  redisReply* get() { return reply_; }

 private:
  redisReply* reply_;
};

using RespPtr = std::unique_ptr<Resp>;
using RespCollection = std::vector<RespPtr>;

class RespDecoder : public ByteToMessageDecoder<std::unique_ptr<RespCollection>> {
 public:
  RespDecoder() : reader_(redisReaderCreate(), redisReaderFree) {}

  bool decode(Context* ctx, IOBufQueue& buf, std::unique_ptr<RespCollection>& result,
              size_t&) override {
    if (buf.chainLength() == 0) {
      return false;
    }
    auto data = buf.move();
    for (auto i = data->begin(); i != data->end(); ++i) {
      if (redisReaderFeed(reader_.get(),
                          reinterpret_cast<const char*>(i->data()),
                          i->size()) != REDIS_OK) {
        fail(ctx, std::string(reader_->errstr));
        return false;
      }
    }

    auto collect = std::make_unique<std::vector<RespPtr>>();
    while (true) {
      redisReply* reply = nullptr;
      if (redisReaderGetReply(reader_.get(), (void**)&reply) != REDIS_OK) {
        fail(ctx, reader_->errstr);
        return false;
      }
      if (reply == nullptr) {
        break;
      }
      collect->push_back(std::make_unique<Resp>(reply));
    }
    if (collect->size() == 0) {
      return false;
    }
    result = std::move(collect);
    return true;
  }

  void fail(Context* ctx, const std::string& msg) {
    ctx->fireReadException(folly::make_exception_wrapper<std::runtime_error>(
        "redis resp decoder error:" + msg));
  }

 private:
  std::unique_ptr<redisReader, void (*)(redisReader*)> reader_;
};