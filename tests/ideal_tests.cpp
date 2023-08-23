#include "ideal_cache.hpp"
#include <cstdlib>
#include <gtest/gtest.h>
#include <iostream>
#include <list>

class CacheFixture : public ::testing::Test {
public:
  const std::size_t       cache_sz = 5;
  const std::size_t       nreqs = 10;
  std::vector<int>        reqs;
  caches::ideal_t<int>   *cache;
  caches::slow_get_page_t slow_get_page{};

protected:
  void SetUp() override {
    for (std::size_t i = 0; i < nreqs; i++) {
      reqs.push_back(i);
    }
    cache = new caches::ideal_t<int>{cache_sz, reqs.begin(), reqs.end()};
  }
  void TearDown() override { delete cache; }
};

TEST_F(CacheFixture, init_test) {
  ASSERT_EQ(cache->getSize(), cache_sz);
  ASSERT_EQ(cache->getIndex(), 0);
  std::vector<int> vec = cache->getVect();
  for (std::size_t i = 0; i < nreqs; i++) {
    ASSERT_EQ(vec[i], reqs[i]);
  }
}

TEST_F(CacheFixture, full_test) {
  ASSERT_EQ(cache->isFullCache(), false);

  std::size_t res = cache->countHits();
  ASSERT_EQ(res, 0);
  ASSERT_EQ(cache->isFullCache(), true);
  ASSERT_EQ(cache->isPresentCache(9), true);
}

TEST(test_1, functional_test_1) {
  std::size_t          cache_sz = 3, ans = 3;
  std::vector<int>     reqs = {1, 2, 3, 1, 2, 3};
  caches::ideal_t<int> cache{cache_sz, reqs.begin(), reqs.end()};
  std::size_t          res = cache.countHits();
  ASSERT_EQ(cache.isFullCache(), true);
  ASSERT_EQ(res, ans);
}

TEST(test_1, functional_test_2) {
  std::size_t          cache_sz = 3, ans = 2;
  std::vector<int>     reqs = {1, 2, 3, 4, 5, 6, 1, 5, 2};
  caches::ideal_t<int> cache{cache_sz, reqs.begin(), reqs.end()};
  std::size_t          res = cache.countHits();
  ASSERT_EQ(cache.isFullCache(), true);
  ASSERT_EQ(res, ans);
}

TEST(test_1, functional_test_3) {
  std::size_t          cache_sz = 3, ans = 3;
  std::vector<int>     reqs = {1, 2, 3, 4, 1, 2, 3, 4};
  caches::ideal_t<int> cache{cache_sz, reqs.begin(), reqs.end()};
  std::size_t          res = cache.countHits();
  ASSERT_EQ(cache.isFullCache(), true);
  ASSERT_EQ(res, ans);
}

TEST(test_1, test_m_reqs) {
  std::size_t      cache_sz = 1000, nreqs = 1000000;
  std::vector<int> reqs;
  for (size_t i = 0; i < nreqs; i++) {
    reqs.push_back(std::rand() % cache_sz);
  }
  caches::ideal_t<int> cache{cache_sz, reqs.begin(), reqs.end()};
  std::size_t          res = cache.countHits();
  EXPECT_GT(res, cache_sz);
}

int main(int argc, char *argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}