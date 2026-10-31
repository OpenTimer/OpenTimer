// This file is distributed under the MIT License. See LICENSE for details.
#pragma once

#include <cassert>
#include <cstddef>
#include <iterator>

namespace ot {
class Arc;

namespace detail {
// Each arc owns two independent links, one for each endpoint. The lists do
// not own arcs: Timer removes both links before destroying the arc itself.
struct Arc_link {
  Arc *value = nullptr;
  Arc_link *previous = nullptr;
  Arc_link *next = nullptr;
};

class Arc_list {
  Arc_link *_head = nullptr;
  Arc_link *_tail = nullptr;
  size_t _size = 0;

public:
  class Iterator {
    const Arc_link *_link = nullptr;

  public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = Arc *;
    using difference_type = std::ptrdiff_t;
    using pointer = Arc *const *;
    using reference = Arc *const &;

    Iterator() = default;
    explicit Iterator(const Arc_link *link) : _link(link) {}
    reference operator*() const { return _link->value; }
    pointer operator->() const { return &_link->value; }
    Iterator &operator++() {
      _link = _link->next;
      return *this;
    }
    Iterator operator++(int) {
      auto prior = *this;
      ++*this;
      return prior;
    }
    bool operator==(const Iterator &) const = default;
  };

  Arc_list() = default;
  Arc_list(const Arc_list &) = delete;
  Arc_list &operator=(const Arc_list &) = delete;
  Iterator begin() const { return Iterator(_head); }
  Iterator end() const { return {}; }
  size_t size() const { return _size; }
  bool empty() const { return _size == 0; }

  void push_back(Arc_link &link, Arc &arc) {
    assert(!link.value && !link.previous && !link.next);
    link.value = &arc;
    link.previous = _tail;
    if (_tail) {
      _tail->next = &link;
    } else {
      _head = &link;
    }
    _tail = &link;
    ++_size;
  }

  void erase(Arc_link &link) {
    assert(link.value && _size);
    if (link.previous) {
      link.previous->next = link.next;
    } else {
      assert(_head == &link);
      _head = link.next;
    }
    if (link.next) {
      link.next->previous = link.previous;
    } else {
      assert(_tail == &link);
      _tail = link.previous;
    }
    link = {};
    --_size;
  }
};
} // namespace detail
} // namespace ot
