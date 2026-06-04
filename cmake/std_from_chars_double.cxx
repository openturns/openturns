#include <charconv>
#include <string>

int main(int /*argc*/, const char* /*argv*/[])
{
  double x = 0.0;
  const std::string pStr = "4.2";
  std::from_chars(pStr.data(), pStr.data() + pStr.size(), x);
  return 0;
}
