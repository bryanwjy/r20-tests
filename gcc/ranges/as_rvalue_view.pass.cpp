// Copyright 2025 Bryan Wong
// Adapted from GCC testsuite

// Copyright (C) 2020-2025 Free Software Foundation, Inc.
//
// This file is part of the GNU ISO C++ Library.  This library is free
// software; you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the
// Free Software Foundation; either version 3, or (at your option)
// any later version.

// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

#include "rxx/ranges/as_rvalue_view.h"

#include "../test_iterators.h"
#include "rxx/algorithm.h"
#include "rxx/ranges/transform_view.h"

#include <cassert>
#include <memory>
#include <optional>
#include <utility>

namespace xranges = rxx::ranges;
namespace xviews = rxx::views;

template <typename T>
struct MoveOnly {
    constexpr MoveOnly(auto&&... args)
        : val{std::in_place, std::forward<decltype(args)>(args)...} {}
    constexpr MoveOnly(MoveOnly const&) = delete;
    constexpr MoveOnly& operator=(MoveOnly const&) = delete;
    constexpr MoveOnly(MoveOnly&& other) : val{std::move(other.val)} {
        other.val.reset();
    }
    constexpr MoveOnly& operator=(MoveOnly&& other) {
        val = std::move(other.val);
        other.val.reset();
        return *this;
    }

    constexpr T* get() const noexcept {
        return val ? const_cast<T*>(&*val) : nullptr;
    }
    constexpr T& operator*() const noexcept { return *get(); }

    std::optional<T> val;
};

template <typename T, typename... Args>
requires (!std::is_array_v<T>)
constexpr auto MakeMoveOnly(Args&&... args) {
    return MoveOnly<T>{std::forward<Args>(args)...};
}

constexpr bool test01() {

    MoveOnly<int> a[3] = {
        MakeMoveOnly<int>(1), MakeMoveOnly<int>(2), MakeMoveOnly<int>(3)};
    MoveOnly<int> b[3];
    auto v = a | xviews::as_rvalue;
    xranges::copy(v, b);
    assert(xranges::all_of(a, [](auto& p) { return p.get() == nullptr; }));
    assert(xranges::equal(
        b | xviews::transform([](auto& p) { return *p; }), (int[]){1, 2, 3}));

    return true;
}

void test02() {
    MoveOnly<int> x = MakeMoveOnly<int>(42);
    MoveOnly<int> y;
    rxx::tests::test_input_range<MoveOnly<int>> rx(&x, &x + 1);
    auto v = rx | xviews::as_rvalue;
    static_assert(!xranges::common_range<decltype(v)>);
    xranges::copy(v, &y);
    assert(x.get() == nullptr);
    assert(*y == 42);
}

int main() {
    static_assert(test01());
    test02();
}
