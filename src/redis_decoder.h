#pragma once

#include <hiredis/hiredis.h>
#include <wangle/codec/ByteToMessageDecoder.h>
#include <wangle/codec/MessageToByteEncoder.h>

#include <algorithm>
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
using ReplyCollection = std::unique_ptr<std::vector<RedisReplyPtr>>;

class ByteToRespDecoder : public ByteToMessageDecoder<ReplyCollection> {
 public:
  ByteToRespDecoder() : reader_(redisReaderCreate(), redisReaderFree) {}

  bool decode(Context* ctx, IOBufQueue& buf, ReplyCollection& result,
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

    // if (redisReaderFeed(reader_.get(), (const char*)data->data(),
    //                     data->length()) != REDIS_OK) {
    //   fail(ctx, std::string(reader_->errstr));
    //   return false;
    // }

    auto collect = std::make_unique<std::vector<RedisReplyPtr>>();
    while (true) {
      redisReply* reply = nullptr;
      if (redisReaderGetReply(reader_.get(), (void**)&reply) != REDIS_OK) {
        fail(ctx, reader_->errstr);
        return false;
      }
      if (reply == nullptr) {
        break;
      }
      collect->push_back(std::make_unique<RedisReply>(reply));
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