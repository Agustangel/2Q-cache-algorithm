#pragma once

#include <iostream>
#include <iterator>
#include <list>
#include <map>
#include <vector>

namespace caches {

template <typename T> class ideal_t {
  std::size_t hits_, size_;

  std::vector<T>           reqs_;
  std::map<std::size_t, T> hash_;
  std::list<T>             cache_;

  using ListIt = typename std::list<T>::iterator;

  ListIt findLatestUsed() {}
  void   lookupUpdate(const T &elem) {}

public:
  template <typename iterator_t>
  ideal_t(std::size_t size, iterator_t begin_t, iterator_t end_t) : reqs_{}, hash_{}, cache_{}, size_{size}, hits_{} {
    std::copy(begin_t, end_t, std::back_inserter(reqs_));
  }

  std::size_t countHits() {
    for (const auto &elem : reqs_) {
      lookupUpdate(elem);
    }
    return hits_;
  }
};
} // namespace caches