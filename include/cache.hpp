#pragma once

#include <iterator>
#include <list>
#include <unordered_map>

namespace caches {

#define CACHE_MIN_SIZE 4

template <typename KeyT, typename T> struct node_t {
  KeyT m_key;
  T    m_value;
  T   *m_ptr;

  node_t(KeyT p_key, T p_val) : m_key{p_key}, m_value{p_val} {}
  node_t(KeyT p_key, T *p_ptr = nullptr) : m_key{p_key}, m_ptr{p_ptr} {}
};
// TODO: Написать документацию
template <typename KeyT, typename T> class cache_t {
  size_t size_;

  using node_t__ = node_t<KeyT, T>;
  using ListIt = typename std::list<node_t__>::iterator;

  // TODO: что если size_ < 4?
  std::list<node_t__> am_; // max_size = 1/2 * size_
  std::list<node_t__> a1_; // max_size = 1/4 * size_
  std::list<node_t__> a2_; // max_size = 1/4 * size_

  std::unordered_map<KeyT, ListIt> am_hash_;
  std::unordered_map<KeyT, ListIt> a1_hash_;
  std::unordered_map<KeyT, ListIt> a2_hash_;

  bool isFullCache() const { return (am_.size() == size_ / 2); }
  bool isFullFIFOin() const { return (a1_.size() == size_ / 4); }
  bool isFullFIFOout() const { return (a2_.size() == size_ / 4); }

  void spliceUpfront(const KeyT &key) {
    auto eltit = am_hash_.find(key)->second;
    if (eltit != am_.begin())
      am_.splice(am_.begin(), am_, eltit, std::next(eltit)); // Move p_elem to the beginning of the am_.
  }

  void reclaimForFIFOout(const KeyT &key) {
    if (isFullCache()) {
      am_hash_.erase(am_.end()->m_key);
      am_.pop_back();
    }
    auto   eltit = a2_hash_.find(key)->second;
    node_t new_page{eltit->m_key, *(eltit->m_ptr)};

    am_.emplace_front(new_page);
    am_hash_.emplace(key, am_.begin());
    a2_.erase(eltit);
    a2_hash_.erase(key);
  }
  template <typename F> void reclaimForFIFOin(const KeyT &key, F &slow_get_page) {
    if (isFullFIFOin()) {
      if (isFullFIFOout()) {
        a2_hash_.erase(a2_.end()->m_key);
        a2_.pop_back();
      }
      auto   eltit = a1_.end();
      node_t new_page{eltit->m_key, &(eltit->m_value)};

      a2_.emplace_front(new_page);
      a2_hash_.emplace(eltit->m_key, a2_.begin());
      a1_hash_.erase(eltit->m_key);
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
      reclaimForFIFOin(key, slow_get_page);
      return false;
    }
  }
};

} // namespace caches