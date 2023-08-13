#pragma once

#include <chrono>
#include <iostream>
#include <iterator>
#include <list>
#include <thread>
#include <unordered_map>
#include <vector>

namespace caches {

#define CACHE_MIN_SIZE 4

// slow get page imitation
struct slow_get_page_t {
  int operator()(int p_key) {
    std::this_thread::sleep_for(std::chrono::milliseconds(1)); // 0.001c
    return p_key;
  }
};

template <typename KeyT, typename T> struct node_t {
  KeyT m_key;
  T    m_value;
  T   *m_ptr;

  node_t(KeyT p_key, T p_val) : m_key{p_key}, m_value{p_val} {}
  node_t(KeyT p_key, T *p_ptr = nullptr) : m_key{p_key}, m_ptr{p_ptr} {}
};

template <typename KeyT, typename T> class cache_t {
  std::size_t hits_, size_;

  using node_t__ = node_t<KeyT, T>;
  using ListIt = typename std::list<node_t__>::iterator;

  std::list<node_t__> lru_;      // max_size = 1/2 * size_
  std::list<node_t__> fifo_in_;  // max_size = 1/4 * size_
  std::list<node_t__> fifo_out_; // max_size = 1/4 * size_

  std::unordered_map<KeyT, ListIt> lru_hash_;
  std::unordered_map<KeyT, ListIt> fifo_in_hash_;
  std::unordered_map<KeyT, ListIt> fifo_out_hash_;

public:
  bool isFullLRU() const { return (lru_.size() == size_); }
  bool isFullFIFOin() const { return (fifo_in_.size() == size_ / 2); }
  bool isFullFIFOout() const { return (fifo_out_.size() == size_ / 4); }

  size_t getSize() const { return size_; }
  size_t getLRUSize() const { return lru_.size(); }
  size_t getFIFOinSize() const { return fifo_in_.size(); }
  size_t getFIFOoutSize() const { return fifo_out_.size(); }

  void dump() const {
    std::cout << "Cache size: " << size_ << std::endl;
    std::cout << "LRU size: " << lru_.size() << std::endl;
    std::cout << "content lru_: " << std::endl;
    for (const auto &elem : lru_) {
      std::cout << elem.m_value << " ";
    }
    std::cout << std::endl;
    std::cout << "FIFOin size: " << fifo_in_.size() << std::endl;
    std::cout << "content fifo_in_: " << std::endl;
    for (const auto &elem : fifo_in_) {
      std::cout << elem.m_value << " ";
    }
    std::cout << std::endl;
    std::cout << "FIFOout size: " << fifo_out_.size() << std::endl;
    std::cout << "content fifo_out_: " << std::endl;
    for (const auto &elem : fifo_out_) {
      std::cout << *elem.m_ptr << " ";
    }
  }

private:
  void spliceUpfront(const KeyT &key) {
    auto eltit = lru_hash_.find(key)->second;
    if (eltit != lru_.begin()) lru_.splice(lru_.begin(), lru_, eltit, std::next(eltit));
  }

  void reclaimForFIFOout(const KeyT &key) {
    if (isFullLRU()) {
      lru_hash_.erase(lru_.end()->m_key);
      lru_.pop_back();
    }
    auto   eltit = fifo_out_hash_.find(key)->second;
    node_t new_page{eltit->m_key, *(eltit->m_ptr)};

    lru_.emplace_front(new_page);
    lru_hash_.emplace(key, lru_.begin());
    fifo_out_.erase(eltit);
    fifo_out_hash_.erase(key);
  }
  template <typename F> void reclaimForFIFOin(const KeyT &key, F &slow_get_page) {
    if (isFullFIFOin()) {
      if (isFullFIFOout()) {
        fifo_out_hash_.erase(fifo_out_.end()->m_key);
        fifo_out_.pop_back();
      }
      auto   eltit = fifo_in_.end();
      node_t new_page{eltit->m_key, &(eltit->m_value)};

      fifo_out_.emplace_front(new_page);
      fifo_out_hash_.emplace(eltit->m_key, fifo_out_.begin());
      fifo_in_hash_.erase(eltit->m_key);
      fifo_in_.pop_back();
    }
    fifo_in_.emplace_front(key, slow_get_page(key));
    fifo_in_hash_.emplace(key, fifo_in_.begin());
  }

  bool isPresentAm(const KeyT &key) const { return (lru_hash_.find(key) != lru_hash_.end()); }
  bool isPresentA1(const KeyT &key) const { return (fifo_in_hash_.find(key) != fifo_in_hash_.end()); }
  bool isPresentA2(const KeyT &key) const { return (fifo_out_hash_.find(key) != fifo_out_hash_.end()); }

public:
  explicit cache_t(size_t size)
      : lru_{}, fifo_in_{}, fifo_out_{}, lru_hash_{}, fifo_in_hash_{}, fifo_out_hash_{}, hits_{},
        size_(size < CACHE_MIN_SIZE ? CACHE_MIN_SIZE : size) {}

  template <typename F> bool lookupUpdate(const KeyT &key, F &slow_get_page) {
#ifdef DEBUG
    dump();
#endif
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
  template <typename F> std::size_t countHits(std::vector<KeyT> &vec, F &slow_get_page) {
    for (const auto &elem : vec) {
      if (lookupUpdate(elem, slow_get_page)) hits_++;
    }
    return hits_;
  }
};

} // namespace caches