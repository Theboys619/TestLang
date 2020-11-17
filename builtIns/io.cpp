#include <iostream>
#include <string>

template <typename T>
void print(T msg, std::string ending = "\n") {
  std::cout << msg << ending;
};