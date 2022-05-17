#include <folly/Format.h>

#include <iostream>

using folly::format;

int main() {
  std::cout << format("The answers are {} and {}\n", 23, 42);
  std::cout << "Hello, World!" << std::endl;
}