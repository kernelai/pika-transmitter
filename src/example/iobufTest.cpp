
#include <folly/io/IOBufQueue.h>

#include <iostream>
#include <memory>
#include <string>

using namespace folly;
using namespace folly::io;

void foo() {
  IOBufQueue q(IOBufQueue::cacheChainLength());
  q.append(IOBuf::copyBuffer("hello"));
  q.append(IOBuf::copyBuffer("world!"));
  std::unique_ptr<IOBuf> buf = q.move();
  std::for_each(buf->begin(), buf->end(), [](auto& range) -> bool {
    std::cout << std::string(range) << std::endl;
    return true;
  });
}

int main(int argc, char** argv) {
  IOBufQueue q(IOBufQueue::cacheChainLength());
  q.append(IOBuf::copyBuffer("hello"));
  q.append(IOBuf::copyBuffer("world!"));
  std::unique_ptr<IOBuf> buf = q.move();
  std::for_each(buf->begin(), buf->end(), [](auto& range) {
    std::cout << std::string(range) << std::endl;
  });
  return 0;
}