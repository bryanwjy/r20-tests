// Copyright 2025 Bryan Wong
// Adapted from LLVM testsuite

//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "rxx/as_rvalue_view.h"
#include <cassert>
#include <cstddef>

struct ConstSizedView : std::ranges::view_base {
  bool *size_called;
  int *begin() const;
  int *end() const;

  constexpr std::size_t size() const {
    *size_called = true;
    return 3;
  }
};

struct SizedView : std::ranges::view_base {
  bool *size_called;
  int *begin() const;
  int *end() const;

  constexpr int size() {
    *size_called = true;
    return 5;
  }
};

struct UnsizedView : std::ranges::view_base {
  int *begin() const;
  int *end() const;
};

template <class T>
concept HasSize = requires(T v) { v.size(); };

static_assert(HasSize<ConstSizedView>);
static_assert(HasSize<const ConstSizedView>);
static_assert(HasSize<SizedView>);
static_assert(!HasSize<const SizedView>);
static_assert(!HasSize<UnsizedView>);
static_assert(!HasSize<const UnsizedView>);

constexpr bool test() {
  {
    bool size_called = false;
    rxx::ranges::as_rvalue_view view(ConstSizedView{{}, &size_called});
    std::same_as<std::size_t> auto size = view.size();
    assert(size == 3);
    assert(size_called);
  }

  {
    bool size_called = false;
    rxx::ranges::as_rvalue_view view(SizedView{{}, &size_called});
    std::same_as<int> auto size = view.size();
    assert(size == 5);
    assert(size_called);
  }

  return true;
}

int main(int, char **) {
  test();
  static_assert(test());

  return 0;
}
