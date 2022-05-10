#include <glog/logging.h>

#include <algorithm>
#include <iostream>
#include <iterator>

void echo() {
  std::copy(std::istream_iterator<char>(std::cin),
            std::istream_iterator<char>(),
            std::ostream_iterator<char>(std::cout, " "));
}

void InitGlog(char* argv[]) { google::InitGoogleLogging(argv[0]); }

void SayHello() { LOG(INFO) << "Glog say: Hello, World!"; }

int main(int argc, char* argv[]) {
  SayHello();
  echo();
}
