#include <atomic>

std::atomic<int> x;

int main() {
  return ++x;
}
