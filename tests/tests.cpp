#pragma once
#include "cache.hpp"
#include <cstdlib>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <iostream>
#include <list>

class CacheFixture : public ::testing::Test {
  const size_t               cache_sz = 8;
  const size_t               nreqs = 10;
  caches::cache_t<int, int> *cache;

protected:
  void SetUp() override {
    std::list<int> reqs;
    for (size_t i = 0; i < nreqs; i++) {
      reqs.push_back(i * 2);
    }
    cache = new cache_2q::cache_t<int, int>(cache_sz);
  }
  void TearDown() override { delete cache; }
};

TEST_F(CacheFixture, init_test) {
  ASSERT_GE(cache->getSize(), cache_sz); // if(cache_sz < CACHE_MIN_SIZE) {cache->size = CACHE_MIN_SIZE}
  (*cache)::testing::StaticAssertTypeEq<int, T>();
}

int main(int argc, char *argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}