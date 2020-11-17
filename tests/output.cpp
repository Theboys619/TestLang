#include <iostream>
#include <string>

template <typename T>
void print(T msg, std::string ending = "\n") {
  std::cout << msg << ending;
};


int main(int argc, char** argv) {
  print(5.5);
};