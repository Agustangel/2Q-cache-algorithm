#include "cache.hpp"
#include <cstdlib>
#include <gtest/gtest.h>
#include <iostream>
#include <list>

class CacheFixture : public ::testing::Test {
public:
  const size_t               cache_sz = 8;
  const size_t               nreqs = 10;
  std::vector<int>           reqs;
  caches::cache_t<int, int> *cache;
  caches::slow_get_page_t    slow_get_page{};

protected:
  void SetUp() override {
    for (size_t i = 0; i < nreqs; i++) {
      reqs.push_back(i * 2);
    }
    cache = new caches::cache_t<int, int>{cache_sz};
  }
  void TearDown() override { delete cache; }
};

TEST_F(CacheFixture, init_test) {
  ASSERT_GE(cache->getSize(), cache_sz); // if(cache_sz < CACHE_MIN_SIZE) {cache->size = CACHE_MIN_SIZE}
}

TEST_F(CacheFixture, addition_test) {
  bool res = cache->lookupUpdate(100, slow_get_page);
  EXPECT_EQ(res, false);
  ASSERT_EQ(cache->getFIFOinSize(), 1);
}

TEST_F(CacheFixture, full_test) {
  ASSERT_EQ(cache->isFullLRU(), false);
  ASSERT_EQ(cache->isFullFIFOin(), false);
  ASSERT_EQ(cache->isFullFIFOout(), false);

  int res = cache->countHits(reqs, slow_get_page);
  ASSERT_EQ(res, 0);
  ASSERT_EQ(cache->isFullLRU(), false);
  ASSERT_EQ(cache->isFullFIFOin(), true);
  ASSERT_EQ(cache->isFullFIFOout(), true);
}

TEST(test_1, test_fifo_mode) {
  std::size_t               cache_sz = 3, ans = 3;
  std::vector<int>          reqs = {1, 2, 3, 1, 2, 3};
  caches::cache_t<int, int> cache{cache_sz};
  caches::slow_get_page_t   slow_get_page{};
  std::size_t               res = cache.countHits(reqs, slow_get_page);
  ASSERT_EQ(cache.isFullFIFOin(), true);
  ASSERT_EQ(res, ans);
}

TEST(test_1, test_m_reqs) {
  std::size_t      cache_sz = 1000, nreqs = 1000000;
  std::vector<int> reqs;
  for (size_t i = 0; i < nreqs; i++) {
    reqs.push_back(std::rand() % cache_sz);
  }
  caches::cache_t<int, int> cache{cache_sz};
  caches::slow_get_page_t   slow_get_page{};
  std::size_t               res = cache.countHits(reqs, slow_get_page);
  EXPECT_GT(res, cache_sz);
}

int main(int argc, char *argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}