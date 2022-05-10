#include <glog/logging.h>

#include <algorithm>
#include <filesystem>
#include <iostream>
#include <iterator>

void echo() {
  std::copy(std::istream_iterator<char>(std::cin),
            std::istream_iterator<char>(),
            std::ostream_iterator<char>(std::cout, " "));
}

void CreateLogDir() {
  try {
    std::filesystem::create_directory(FLAGS_log_dir);
  } catch (const std::exception& e) {
    LOG(ERROR) << "mkdir log directory error: " << e.what() << '\n';
  }
}

void InitGlog(char* argv[]) {
  google::InitGoogleLogging(argv[0]);
  FLAGS_log_dir = "./log";
  CreateLogDir();
  google::EnableLogCleaner(3);
}

void SayHello() {
  LOG(INFO) << "Glog say: Hello, World!";
  LOG(WARNING) << "Glog say: Hello, World!";
}

// int main(int argc, char* argv[]) {
//   InitGlog(argv);
//   SayHello();
//   echo();
// }
