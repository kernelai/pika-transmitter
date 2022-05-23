#pragma once  

#include <hiredis/hiredis.h>
#include <wangle/codec/ByteToMessageDecoder.h>
#include <wangle/codec/MessageToByteEncoder.h>

using RedisReplyPtr = std::unique_ptr<redisReply, void (*)(void*)>;
class ByteToRespDecoder
    : public ByteToMessageDecoder<
          std::unique_ptr<redisReply, void (*)(void* reply)>> {
 public:
  ByteToRespDecoder() : reader_(redisReaderCreate(), redisReaderFree) {}

  bool decode(Context*, IOBufQueue& buf,
              RedisReplyPtr result,
              size_t&) override {
    if (buf.chainLength() > 0) {
      redisReaderFeed(reader_.get(), buf.front()->data(), buf.chainLength());
      return true;
    }
    return false;
  }

 private:
  std::unique_ptr<redisReader, void (*)(redisReader*)> reader_;
};