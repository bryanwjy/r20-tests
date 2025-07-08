// Copyright 2025 Bryan Wong
// Adapted from LLVM testsuite

//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef MOVEONLY_H
#define MOVEONLY_H

#include <cstddef>
#include <functional>

class MoveOnly {
  int data_;

public:
  constexpr MoveOnly(int data = 1) : data_(data) {}

  MoveOnly(const MoveOnly &) = delete;
  MoveOnly &operator=(const MoveOnly &) = delete;

  constexpr MoveOnly(MoveOnly &&x) noexcept : data_(x.data_) { x.data_ = 0; }
  constexpr MoveOnly &operator=(MoveOnly &&x) {
    data_ = x.data_;
    x.data_ = 0;
    return *this;
  }

  constexpr int get() const { return data_; }

  friend constexpr bool operator==(const MoveOnly &x, const MoveOnly &y) {
    return x.data_ == y.data_;
  }
  friend constexpr bool operator!=(const MoveOnly &x, const MoveOnly &y) {
    return x.data_ != y.data_;
  }
  friend constexpr bool operator<(const MoveOnly &x, const MoveOnly &y) {
    return x.data_ < y.data_;
  }
  friend constexpr bool operator<=(const MoveOnly &x, const MoveOnly &y) {
    return x.data_ <= y.data_;
  }
  friend constexpr bool operator>(const MoveOnly &x, const MoveOnly &y) {
    return x.data_ > y.data_;
  }
  friend constexpr bool operator>=(const MoveOnly &x, const MoveOnly &y) {
    return x.data_ >= y.data_;
  }

  friend constexpr auto operator<=>(const MoveOnly &,
                                    const MoveOnly &) = default;

  constexpr MoveOnly operator+(const MoveOnly &x) const {
    return MoveOnly(data_ + x.data_);
  }
  constexpr MoveOnly operator*(const MoveOnly &x) const {
    return MoveOnly(data_ * x.data_);
  }

  template <class T> friend void operator,(MoveOnly const &, T) = delete;

  template <class T> friend void operator,(T, MoveOnly const &) = delete;
};

template <> struct std::hash<MoveOnly> {
  typedef MoveOnly argument_type;
  typedef std::size_t result_type;
  constexpr std::size_t operator()(const MoveOnly &x) const {
    return static_cast<size_t>(x.get());
  }
};

class TrivialMoveOnly {
  int data_;

public:
  constexpr TrivialMoveOnly(int data = 1) : data_(data) {}

  TrivialMoveOnly(const TrivialMoveOnly &) = delete;
  TrivialMoveOnly &operator=(const TrivialMoveOnly &) = delete;

  TrivialMoveOnly(TrivialMoveOnly &&) = default;
  TrivialMoveOnly &operator=(TrivialMoveOnly &&) = default;

  constexpr int get() const { return data_; }

  friend constexpr bool operator==(const TrivialMoveOnly &x,
                                   const TrivialMoveOnly &y) {
    return x.data_ == y.data_;
  }
  friend constexpr bool operator!=(const TrivialMoveOnly &x,
                                   const TrivialMoveOnly &y) {
    return x.data_ != y.data_;
  }
  friend constexpr bool operator<(const TrivialMoveOnly &x,
                                  const TrivialMoveOnly &y) {
    return x.data_ < y.data_;
  }
  friend constexpr bool operator<=(const TrivialMoveOnly &x,
                                   const TrivialMoveOnly &y) {
    return x.data_ <= y.data_;
  }
  friend constexpr bool operator>(const TrivialMoveOnly &x,
                                  const TrivialMoveOnly &y) {
    return x.data_ > y.data_;
  }
  friend constexpr bool operator>=(const TrivialMoveOnly &x,
                                   const TrivialMoveOnly &y) {
    return x.data_ >= y.data_;
  }

  friend constexpr auto operator<=>(const TrivialMoveOnly &,
                                    const TrivialMoveOnly &) = default;

  constexpr TrivialMoveOnly operator+(const TrivialMoveOnly &x) const {
    return TrivialMoveOnly(data_ + x.data_);
  }
  constexpr TrivialMoveOnly operator*(const TrivialMoveOnly &x) const {
    return TrivialMoveOnly(data_ * x.data_);
  }

  template <class T> friend void operator,(TrivialMoveOnly const &, T) = delete;

  template <class T> friend void operator,(T, TrivialMoveOnly const &) = delete;
};

template <> struct std::hash<TrivialMoveOnly> {
  typedef TrivialMoveOnly argument_type;
  typedef std::size_t result_type;
  constexpr std::size_t operator()(const TrivialMoveOnly &x) const {
    return static_cast<size_t>(x.get());
  }
};

#endif // MOVEONLY_H
