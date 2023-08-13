#include <cassert>
#include <iostream>
#include <vector>

#include "cache.hpp"

int main() {
  int         hits{};
  std::size_t n{}, cache_size{};

  std::cin >> n >> cache_size;
  assert(std::cin.good());
  if (cache_size < CACHE_MIN_SIZE) cache_size = CACHE_MIN_SIZE;

  std::vector<int> vec{};
  vec.reserve(n);

  for (size_t i = 0; i < n; i++) {
    int temp{};
    std::cin >> temp;
    assert(std::cin.good());

    vec.push_back(temp);
  }

  caches::cache_t<int, int> cache{cache_size};
  caches::slow_get_page_t   g{};

  std::cout << cache.countHits(vec, g) << std::endl;

  return 0;
}