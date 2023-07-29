#include <cassert>
#include <chrono>
#include <iostream>
#include <thread>

#include "cache.hpp"

// slow get page imitation
int slow_get_page(int page) {
  std::this_thread::sleep_for(std::chrono::milliseconds(1)); // 0.001c
  return page;
}

int main() {
  int    hits = 0;
  int    number_requests;
  size_t cache_size;

  std::cin >> number_requests >> cache_size;
  assert(std::cin.good());
  caches::cache_t<int> cache{cache_size};

  for (int i = 0; i < number_requests; ++i) {
    int key;
    std::cin >> key;
    assert(std::cin.good());
    if (cache.lookupUpdate(key, slow_get_page)) hits += 1;
  }

  std::cout << hits << std::endl;

  return 0;
}