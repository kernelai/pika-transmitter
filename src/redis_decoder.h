#pragma once

#include <hiredis/hiredis.h>
#include <wangle/codec/ByteToMessageDecoder.h>
#include <wangle/codec/MessageToByteEncoder.h>

#include <string>
using namespace wangle;
using namespace folly;

class RedisReply {
 public:
  RedisReply(redisReply* reply) : reply_(reply) {}
  ~RedisReply() {
    if (reply_) {
      freeReplyObject(reply_);
    }
  }
  redisReply* reply() { return reply_; }

 private:
  redisReply* reply_;
};

using RedisReplyPtr = std::unique_ptr<RedisReply>;

class ByteToRespDecoder : public ByteToMessageDecoder<RedisReplyPtr> {
 public:
  ByteToRespDecoder() : reader_(redisReaderCreate(), redisReaderFree) {}

  bool decode(Context* ctx, IOBufQueue& buf, RedisReplyPtr& result,
              size_t&) override {
    if (buf.chainLength() == 0) {
      return false;
    }
    auto data = buf.move();
    if (redisReaderFeed(reader_.get(), (const char*)data->data(),
                        data->length()) != REDIS_OK) {
      fail(ctx, std::string(reader_->errstr));
      return false;
    }
    redisReply* reply = nullptr;
    if (redisReaderGetReply(reader_.get(), (void**)&reply) != REDIS_OK) {
      fail(ctx, reader_->errstr);
      return false;
    }
    if (reply == nullptr) {
      return false;
    }
    result = std::make_unique<RedisReply>(reply);
    return true;
  }

  void fail(Context* ctx, const std::string& msg) {
    ctx->fireReadException(folly::make_exception_wrapper<std::runtime_error>(
        "redis resp decoder error:" + msg));
  }

 private:
  std::unique_ptr<redisReader, void (*)(redisReader*)> reader_;
};