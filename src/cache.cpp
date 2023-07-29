#include <cassert>
#include <chrono>
#include <iostream>
#include <thread>
#include <vector>

#include "cache.hpp"

// slow get page imitation
struct slow_get_page {
  int operator()(int p_key) {
    std::this_thread::sleep_for(std::chrono::milliseconds(1)); // 0.001c
    return p_key;
  }
};

int main() {
  int    hits{};
  size_t n{}, cache_size{};

  std::cin >> n >> cache_size;
  assert(std::cin.good());
  if (cache_size < CACHE_MIN_SIZE) cache_size = CACHE_MIN_SIZE;

  std::vector<int> vec{};
  vec.reserve(n);

  for (unsigned i = 0; i < n; ++i) {
    int temp{};
    std::cin >> temp;
    assert(std::cin.good());

    vec.push_back(temp);
  }

  caches::cache_t<int, int> cache{cache_size};
  slow_get_page             g{};

  for (const auto &elem : vec) {
    if (cache.lookupUpdate(elem, g)) hits++;
  }

  std::cout << hits << std::endl;

  return 0;
}