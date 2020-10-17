#include <regex>

int main() {
  return std::regex_match("zou2", std::regex("[a-zA-Z][0-9a-zA-Z_]*"));
}
