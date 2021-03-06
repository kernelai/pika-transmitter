#include <folly/portability/GTest.h>

#include "RespDecoder.h"

using namespace folly;
using namespace wangle;

class FrameTester : public wangle::InboundHandler<std::unique_ptr<RespCollection>> {
 public:
  explicit FrameTester(folly::Function<void(std::unique_ptr<RespCollection>)> test)
      : test_(std::move(test)) {}

  void read(Context*, std::unique_ptr<RespCollection> buf) override { test_(std::move(buf)); }

  void readException(Context*, folly::exception_wrapper e) override {
    LOG(ERROR) << "readException" << e.what();
  }

 private:
  folly::Function<void(std::unique_ptr<RespCollection>)> test_;
};

TEST(RespDecoder, Simple) {
  auto pipeline = Pipeline<IOBufQueue&, std::unique_ptr<IOBuf>>::create();
  int called = 0;

  (*pipeline)
      .addBack(RespDecoder())
      .addBack(FrameTester([&](std::unique_ptr<RespCollection> collect) {
        called++;
        EXPECT_EQ(2, collect->size());
        for_each(collect->begin(), collect->end(), [](auto& resp) {
          EXPECT_EQ(REDIS_REPLY_STATUS, resp->get()->type);
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
