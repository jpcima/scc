#pragma once
#include <memory>
#include <stdexcept>
#include <cstddef>
#include <cassert>

template <class T>
class CCircularBuffer {
public:
  explicit CCircularBuffer(size_t capacity)
    : cap_(capacity + 1), data_(new T[capacity + 1])
  {
  }

  size_t capacity() const
  {
    return cap_ - 1;
  }

  size_t size() const
  {
    const size_t rp = rp_, wp = wp_, cap = cap_;
    return wp + ((wp < rp) ? cap : 0) - rp;
  }

  bool empty() const
  {
    return size() == 0;
  }

  bool full() const
  {
    return size() == cap_ - 1;
  }

  void clear()
  {
    size_t rp = rp_;
    const size_t wp = wp_, cap = cap_;
    while (rp != wp) {
      data_[rp] = T();
      rp = (rp + 1 < cap) ? (rp + 1) : 0;
    }
    rp_ = rp;
  }

  void push(T x)
  {
    assert(!full());
    const size_t wp = wp_, cap = cap_;
    data_[wp] = std::move(x);
    wp_ = (wp + 1 < cap) ? (wp + 1) : 0;
  }

  T pop()
  {
    assert(!empty());
    const size_t rp = rp_, cap = cap_;
    T x = std::move(data_[rp]);
    rp_ = (rp + 1 < cap) ? (rp + 1) : 0;
    return x;
  }

private:
    size_t cap_{0};
    size_t rp_{0}, wp_{0};
    std::unique_ptr<T[]> data_{};
};

template <class T, size_t N>
class CCircularBuffers {
public:
  explicit CCircularBuffers(size_t capacity)
  {
    for (size_t i = 0; i < N; ++i)
      buffers_[i].reset(new CCircularBuffer<T>(capacity));
  }

  CCircularBuffer<T> &operator[](size_t index)
  {
    return *buffers_.at(index);
  }

  const CCircularBuffer<T> &operator[](size_t index) const
  {
    return *buffers_.at(index);
  }

private:
  std::array<std::unique_ptr<CCircularBuffer<T>>, N> buffers_;
};
