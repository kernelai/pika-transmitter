#include <folly/portability/GTest.h>

#include "OperationHandler.h"
#include "RespDecoder.h"

TEST(OperationHandler, Simple) {
  auto pipeline = Pipeline<IOBufQueue&, std::unique_ptr<IOBuf>>::create();
  int called = 0;

  (*pipeline)
      .addBack(RespDecoder())
      .finalize();

  IOBufQueue q(IOBufQueue::cacheChainLength());
  q.append(IOBuf::copyBuffer("+OK\r\n"));
  q.append(IOBuf::copyBuffer("+OK\r\n"));
  pipeline->read(q);
  pipeline->read(q);
  EXPECT_EQ(called, 1);

  //todo check result
}
