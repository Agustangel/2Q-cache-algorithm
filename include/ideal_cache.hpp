#pragma once

#include <iostream>
#include <iterator>
#include <list>
#include <unordered_map>
#include <vector>

namespace caches {

// slow get page imitation
struct slow_get_page_t {
  int operator()(int p_key) { return p_key; }
};

template <typename T, typename KeyT = int> class ideal_t {
  std::size_t hits_, size_;
  std::size_t current_index_;

  using node_t = typename std::pair<KeyT, T>;
  using ListIt = typename std::list<node_t>::iterator;

  std::vector<KeyT> reqs_;
  std::list<node_t> cache_;

  std::unordered_map<KeyT, ListIt>      hash_;
  std::unordered_map<KeyT, std::size_t> map_;

public:
  bool isFullCache() const { return (cache_.size() == size_); }
  bool isPresentCache(const KeyT &key) const { return (hash_.find(key) != hash_.end()); }

  std::size_t getIndex() const { return current_index_; }
  std::size_t getSize() const { return size_; }

  const std::vector<KeyT> &getVect() const { return reqs_; }

private:
  void spliceUpfront(const KeyT &key) {
    auto eltit = hash_.find(key)->second;
    if (eltit != cache_.begin()) cache_.splice(cache_.begin(), cache_, eltit, std::next(eltit));
  }

  KeyT findLatestUsed() const {
    auto latest_used = map_.begin()->first;
    auto latest_index = map_.begin()->second;
    for (auto its = map_.begin(), ite = map_.end(); its != ite; ++its) {
      if (its->second > latest_index) {
        latest_used = its->first;
        latest_index = its->second;
      }
    }
    return latest_used;
  }

  void fillMap() {
    for (auto its = map_.begin(), ite = map_.end(); its != ite; ++its) {
      auto        elem_it = hash_.find(its->first)->second;
      std::size_t elem_index{};

      for (std::size_t i = current_index_, end = reqs_.size(); i != end; ++i) {
        if (reqs_[i] != elem_it->first)
          elem_index++;
        else
          break;
      }
      its->second = elem_index;
    }
  }

  void eraseLatestUsed() {
    fillMap();
    auto latest_used = findLatestUsed();
    auto found = hash_.find(latest_used)->second;

    hash_.erase(latest_used);
    map_.erase(latest_used);
    cache_.erase(found);
  }

  template <typename F> void insertElem(const KeyT &key, F &slow_get_page) {
    cache_.emplace_front(key, slow_get_page(key));
    hash_.emplace(key, cache_.begin());
    map_.emplace(key, 0); // fillMap will then correctly fill in the second field
  }

  template <typename F> void lookupUpdate(const KeyT &key, F &slow_get_page) {
    if (isPresentCache(key)) {
      spliceUpfront(key);
      hits_++;
    } else if (!isFullCache()) {
      insertElem(key, slow_get_page);
    } else {
      eraseLatestUsed();
      insertElem(key, slow_get_page);
    }
  }

public:
  template <typename iterator_t>
  ideal_t(std::size_t size, iterator_t begin_t, iterator_t end_t)
      : reqs_{}, hash_{}, cache_{}, size_{size}, hits_{}, current_index_{} {
    std::copy(begin_t, end_t, std::back_inserter(reqs_));
  }

  std::size_t countHits() {
    slow_get_page_t g{};

    for (const auto &elem : reqs_) {
      lookupUpdate(elem, g);
      current_index_++;
    }
    return hits_;
  }
};
} // namespace caches