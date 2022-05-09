#include <iostream>
#include <iterator>

void echo() {
  std::copy(std::istream_iterator<char>(std::cin),
            std::istream_iterator<char>(),
            std::ostream_iterator<char>(std::cout, " "));
}

int main() {
  std::cout << "Hello, World!" << std::endl;
  echo();
}
