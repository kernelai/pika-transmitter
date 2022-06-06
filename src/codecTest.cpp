#include <folly/portability/GTest.h>

#include "redis_decoder.h"

using namespace folly;
using namespace wangle;
using namespace folly::io;

namespace {
auto createZeroedBuffer(size_t size) {
  auto ret = IOBuf::create(size);
  ret->append(size);
  std::memset(ret->writableData(), 0x00, size);
  return ret;
}
}  // namespace

class FrameTester : public wangle::InboundHandler<ReplyCollection> {
 public:
  explicit FrameTester(folly::Function<void(ReplyCollection)> test)
      : test_(std::move(test)) {}

  void read(Context*, ReplyCollection buf) override { test_(std::move(buf)); }

  void readException(Context*, folly::exception_wrapper e) override {
    LOG(ERROR) << "readException" << e.what();
  }

 private:
  folly::Function<void(ReplyCollection)> test_;
};

TEST(ByteToRespDecoder, Simple) {
  auto pipeline = Pipeline<IOBufQueue&, std::unique_ptr<IOBuf>>::create();
  int called = 0;

  (*pipeline)
      .addBack(ByteToRespDecoder())
      .addBack(FrameTester([&](ReplyCollection collect) { called++;
                              EXPECT_EQ(2, collect->size());
                              for_each(collect->begin(), collect->end(),
                                       [](auto& reply) {
                                         EXPECT_EQ(REDIS_REPLY_STATUS, reply->reply()->type);
                                       });
                            })) 
      .finalize();

  IOBufQueue q(IOBufQueue::cacheChainLength());
  q.append(IOBuf::copyBuffer("+OK\r\n"));
  q.append(IOBuf::copyBuffer("+OK\r\n"));
  pipeline->read(q);
  pipeline->read(q);
  EXPECT_EQ(called, 1);
}
