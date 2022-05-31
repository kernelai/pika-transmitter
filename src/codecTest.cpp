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
} // namespace

class FrameTester
    : public wangle::InboundHandler<RedisReplyPtr> {
 public:
  explicit FrameTester(
      folly::Function<void(RedisReplyPtr)> test)
      : test_(std::move(test)) {}

  void read(Context*, RedisReplyPtr buf) override {
    test_(std::move(buf));
  }

  void readException(Context*, folly::exception_wrapper) override {
    test_(nullptr);
  }

 private:
  folly::Function<void(RedisReplyPtr)> test_;
};

TEST(ByteToRespDecoder, Simple) {
  auto pipeline = Pipeline<IOBufQueue&, std::unique_ptr<IOBuf>>::create();
  int called = 0;

  (*pipeline)
      .addBack(ByteToRespDecoder())
      .addBack(FrameTester([&](RedisReplyPtr reply) {
        called++;
      }))
      .finalize();

  auto buf = createZeroedBuffer(3);

  IOBufQueue q(IOBufQueue::cacheChainLength());

  q.append(std::move(buf));
  pipeline->read(q);
  EXPECT_EQ(called, 0);
  

//   buf = createZeroedBuffer(1);
//   RWPrivateCursor c(buf.get());
//   c.write<char>('\n');
//   q.append(std::move(buf));
//   pipeline->read(q);
//   EXPECT_EQ(called, 1);

//   buf = createZeroedBuffer(4);
//   RWPrivateCursor c1(buf.get());
//   c1.write(' ');
//   c1.write(' ');
//   c1.write(' ');

//   c1.write('\r');
//   q.append(std::move(buf));
//   pipeline->read(q);
//   EXPECT_EQ(called, 1);

//   buf = createZeroedBuffer(1);
//   RWPrivateCursor c2(buf.get());
//   c2.write('\n');
//   q.append(std::move(buf));
//   pipeline->read(q);
//   EXPECT_EQ(called, 2);
}
