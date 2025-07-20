// Copyright 2025 Bryan Wong
// Adapted from LLVM testsuite

//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// UNSUPPORTED: c++03, c++11, c++14, c++17

// friend iter_rvalue_reference_t<I> iter_move(const common_iterator& i)
//   noexcept(noexcept(ranges::iter_move(declval<const I&>())))
//     requires input_iterator<I>;

#include "../static_asserts.h"
#include "../test_iterators.h"
#include "rxx/iterator.h"

#include <cassert>
#include <type_traits>

namespace xranges = rxx::ranges;

struct IterMovingIt {
    using value_type = int;
    using difference_type = int;
    explicit IterMovingIt() = default;
    IterMovingIt(
        IterMovingIt const&); // copyable, but this test shouldn't make copies
    IterMovingIt(IterMovingIt&&) = default;
    IterMovingIt& operator=(IterMovingIt const&);
    int& operator*() const;
    constexpr IterMovingIt& operator++() { return *this; }
    IterMovingIt operator++(int);
    friend constexpr int iter_move(IterMovingIt const&) { return 42; }
    bool operator==(std::default_sentinel_t) const;
};
static_assert(std::input_iterator<IterMovingIt>);

constexpr bool test() {
    {
        using It = int*;
        using CommonIt = std::common_iterator<It, sentinel_wrapper<It>>;
        int a[] = {1, 2, 3};
        CommonIt it = CommonIt(It(a));
        ASSERT_NOEXCEPT(iter_move(it));
        ASSERT_NOEXCEPT(xranges::iter_move(it));
        ASSERT_SAME_TYPE(decltype(iter_move(it)), int&&);
        ASSERT_SAME_TYPE(decltype(xranges::iter_move(it)), int&&);
        assert(iter_move(it) == 1);
        if (!std::is_constant_evaluated()) {
            ++it;
            assert(iter_move(it) == 2);
        }
    }
    {
        using It = int const*;
        using CommonIt = std::common_iterator<It, sentinel_wrapper<It>>;
        int a[] = {1, 2, 3};
        CommonIt it = CommonIt(It(a));
        ASSERT_NOEXCEPT(iter_move(it));
        ASSERT_NOEXCEPT(xranges::iter_move(it));
        ASSERT_SAME_TYPE(decltype(iter_move(it)), int const&&);
        ASSERT_SAME_TYPE(decltype(xranges::iter_move(it)), int const&&);
        assert(iter_move(it) == 1);
        if (!std::is_constant_evaluated()) {
            ++it;
            assert(iter_move(it) == 2);
        }
    }
    {
        using It = IterMovingIt;
        using CommonIt = std::common_iterator<It, std::default_sentinel_t>;
        CommonIt it = CommonIt(It());
        ASSERT_NOT_NOEXCEPT(iter_move(it));
        ASSERT_NOT_NOEXCEPT(xranges::iter_move(it));
        ASSERT_SAME_TYPE(decltype(iter_move(it)), int);
        ASSERT_SAME_TYPE(decltype(xranges::iter_move(it)), int);
        assert(iter_move(it) == 42);
        if (!std::is_constant_evaluated()) {
            ++it;
            assert(iter_move(it) == 42);
        }
    }
    return true;
}

int main(int, char**) {
    test();
    static_assert(test());

    return 0;
}
