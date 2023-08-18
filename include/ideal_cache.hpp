#pragma once

#include <iostream>
#include <iterator>
#include <list>
#include <map>
#include <unordered_map>
#include <vector>

namespace caches {

template <typename T> class ideal_t {
  std::size_t hits_, size_;

  using ListIt = typename std::list<T>::iterator;

  std::vector<T>                reqs_;
  std::map<std::size_t, ListIt> map_;
  std::list<T>                  cache_;

  bool isFullCache() const { return (cache_.size() == size_); }

  void insertElem(const T &elem) {
    if (isFullCache()) {
      auto latest_used = findLatestUsed();
      auto found = map_.find(latest_used)->second;
      cache_.erase(found);
      map_.erase(latest_used);
    }
    cache_.emplace_front(elem);
    reqs_.erase(reqs_.begin());
    map_.emplace(0, cache_.begin()); // fillMap will then correctly fill in the first field
  }
  std::size_t findLatestUsed() const {
    auto latest_used = map_.begin()->first;
    for (auto its = map_.begin(), ite = map_.end(); its != ite; ++its) {
      if (its->first > latest_used) {
        latest_used = its->first;
      }
    }
    return latest_used;
  }

  void fillMap() {
    for (auto its = map_.begin(), ite = map_.end(); its != ite; ++its) {
      auto        elem = *(its->second);
      std::size_t elem_index{};
      for (std::size_t i = 0, end = reqs_.size(); i != end; ++i) {
        if (reqs_[i] != elem) elem_index++;
      }
      its->first = elem_index;
    }
  }
  void lookupUpdate(const T &elem) {}

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