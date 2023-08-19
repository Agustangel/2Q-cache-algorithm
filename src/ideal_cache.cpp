#include <cassert>
#include <iostream>
#include <vector>

#include "ideal_cache.hpp"

int main() {
  int         hits{};
  std::size_t n{}, cache_size{};

  std::cin >> n >> cache_size;
  assert(std::cin.good());

  std::vector<int> vec{};
  vec.reserve(n);

  for (size_t i = 0; i < n; i++) {
    int temp{};
    std::cin >> temp;
    assert(std::cin.good());

    vec.push_back(temp);
  }

  caches::ideal_t<int> cache{cache_size, vec.begin(), vec.end()};
  std::cout << cache.countHits() << std::endl;

  return 0;
}