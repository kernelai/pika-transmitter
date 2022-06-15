#include <folly/portability/GTest.h>

#include "RequestHandler.h"
#include "RespDecoder.h"

using namespace folly;
using namespace wangle;

class FrameTester : public wangle::InboundHandler<std::unique_ptr<Request>> {
 public:
  explicit FrameTester(folly::Function<void(std::unique_ptr<Request>)> test)
      : test_(std::move(test)) {}

  void read(Context*, std::unique_ptr<Request> buf) override { test_(std::move(buf)); }

  void readException(Context*, folly::exception_wrapper e) override {
    LOG(ERROR) << "readException" << e.what();
  }

 private:
  folly::Function<void(std::unique_ptr<Request>)> test_;
};

TEST(RequestHandler, HandlePingRequest) {
  auto pipeline = Pipeline<IOBufQueue&, std::unique_ptr<IOBuf>>::create();
  int called = 0;

  (*pipeline)
      .addBack(RespDecoder())
      .addBack(RequestHandler())
      .addBack(FrameTester([&](std::unique_ptr<Request> request) {
        called++;
        EXPECT_EQ(OperationType::ADMIN, request->type);
        EXPECT_EQ("PING", request->name);
      }))
      .finalize();

  IOBufQueue q(IOBufQueue::cacheChainLength());
  q.append(IOBuf::copyBuffer("$4\r\nPING\r\n"));
  pipeline->read(q);
  EXPECT_EQ(called, 1);

  //todo check result
}
