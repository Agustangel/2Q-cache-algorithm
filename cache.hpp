#pragma once

#include <iterator>
#include <list>
#include <unordered_map>

namespace cache {

template <typename KeyT, typename T> struct node_t {
  KeyT m_key;
  T    m_value;

  node_t(KeyT p_key, T p_val) : m_key{p_key}, m_value{p_val} {}
};
// TODO: переделать алгоритм под тип node_t, выделить отдельные функции. Написать документацию
template <typename KeyT, typename T> class cache_t {
  size_t size_;

  using node_t__ = node_t<KeyT, T>;
  using ListIt = typename std::list<node_t__>::iterator;
  using FIFOIt = typename std::list<node_t__ *>::iterator;

  std::list<node_t__>   am_; // чтобы не было петли
  std::list<node_t__>   a1_; // max_size = 1/4 * size_
  std::list<node_t__ *> a2_; // max_size = 1/2 * size_

  std::unordered_map<KeyT, ListIt> am_hash_;
  std::unordered_map<KeyT, ListIt> a1_hash_;
  std::unordered_map<KeyT, FIFOIt> a2_hash_;

  bool isFullCache() const { return (am_.size() == size_); }
  bool isFullFIFOin() const { return (a1_.size() == size_ / 4); }
  bool isFullFIFOout() const { return (a2_.size() == size_ / 2); }

  void spliceUpfront(const KeyT &key) {
    auto eltit = am_hash_.find(key);
    if (eltit != am_.begin()) am_.splice(am_.begin(), am_, eltit, std::next(eltit));
  }

  void reclaimForFIFOout(const KeyT &key) {
    auto eltit = a2_hash_.find(key);
    if (isFullCache()) {
      hash_.erase(am_.back().first);
      am_.pop_back();
    }
    am_.emplace_front(**eltit);
    hash_.emplace(key, am_.begin());
    a2_hash_.erase(key);
    a2_.erase(eltit);
  }
  template <typename F> void reclaimForFIFOin(const KeyT &key, F slow_get_page) {
    if (isFullFIFOin()) {
      auto page = a1_.back();
      if (isFullFIFOout()) {
        a2_hash_.erase(a2_.back().first);
        a2_.pop_back();
      }
      a2_.emplace_front(&page);
      a2_hash_.emplace(page.first, a2_.begin());
      a1_hash_.erase(page.first);
      a1_.pop_back();
    }
    a1_.emplace_front(key, slow_get_page(key));
    a1_hash_.emplace(key, a1_.begin());
  }

  bool isPresentAm(const KeyT &key) const { return (am_hash_.find(key) != am_hash_.end()); }
  bool isPresentA1(const KeyT &key) const { return (a1_hash_.find(key) != a1_hash_.end()); }
  bool isPresentA2(const KeyT &key) const { return (a2_hash_.find(key) != a2_hash_.end()); }

public:
  explicit cache_t(size_t size) : am_{}, a1_{}, a2_{}, am_hash_{}, a1_hash_{}, a2_hash_{}, size_(size) {}

  template <typename F> bool lookupUpdate(const KeyT &key, F slow_get_page) {
    if (isPresentAm(key)) {
      spliceUpfront(key);
      return true;
    } else if (isPresentA1(key)) {
      return true;
    } else if (isPresentA2(key)) {
      reclaimForFIFOout(key);
      return true;
    } else {
      reclaimForFIFOin(key); // slow_get_page?
      return false;
    }
  }
};

} // namespace cache