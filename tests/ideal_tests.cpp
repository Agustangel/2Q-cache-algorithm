#include "ideal_cache.hpp"
#include <cstdlib>
#include <gtest/gtest.h>
#include <iostream>
#include <list>

class CacheFixture : public ::testing::Test {
public:
  const std::size_t       cache_sz = 8;
  const std::size_t       nreqs = 10;
  std::vector<int>        reqs;
  caches::ideal_t<int>   *cache;
  caches::slow_get_page_t slow_get_page{};

protected:
  void SetUp() override {
    for (std::size_t i = 0; i < nreqs; i++) {
      reqs.push_back(i * 2);
    }
    cache = new caches::ideal_t<int>{cache_sz, reqs.begin(), reqs.end()};
  }
  void TearDown() override { delete cache; }
};

int main(int argc, char *argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}