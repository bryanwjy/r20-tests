// Copyright 2025 Bryan Wong
// Adapted from LLVM testsuite

//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// UNSUPPORTED: c++03, c++11, c++14, c++17, c++20

// <ranges>

// constexpr iterator& operator--();
// constexpr iterator operator--(int);

#include "../../test_iterators.h"
#include "../types.h"
#include "rxx/functional.h"
#include "rxx/ranges/chunk_by_view.h"
#include "rxx/ranges/transform_view.h"

#include <array>
#include <cassert>
#include <concepts>
#include <span>
#include <type_traits>
#include <utility>

template <typename T>
requires std::is_enum_v<T>
static constexpr auto to_underlying(T val) noexcept {
    return static_cast<std::underlying_type_t<T>>(val);
}

namespace xranges = rxx::ranges;
namespace xviews = rxx::views;

template <class T>
concept HasPreDecrement = requires(T t) {
    { --t };
};

template <class T>
concept HasPostDecrement = requires(T t) {
    { t-- };
};

struct TrackingPred : TrackInitialization {
    using TrackInitialization::TrackInitialization;
    constexpr bool operator()(int x, int y) const { return x <= y; }
};

template <class Iter, IsConst Constant, class Sent = sentinel_wrapper<Iter>>
constexpr void test() {
    using Underlying = View<Iter, Sent>;
    using ChunkByView = xranges::chunk_by_view<Underlying, xranges::less_equal>;
    using ChunkByIterator = xranges::iterator_t<ChunkByView>;

    static_assert(HasPostDecrement<ChunkByIterator>);
    static_assert(HasPreDecrement<ChunkByIterator>);

    auto make_chunk_by_view = [](auto& arr) {
        View view{Iter{arr.data()}, Sent{Iter{arr.data() + arr.size()}}};
        return ChunkByView{std::move(view), xranges::less_equal{}};
    };

    // Test with a single chunk
    {
        std::array array{0, 1, 2, 3, 4};
        ChunkByView view = make_chunk_by_view(array);
        ChunkByIterator it = xranges::next(view.begin(), view.end());

        std::same_as<ChunkByIterator&> decltype(auto) result = --it;
        assert(&result == &it);
        assert(base((*result).begin()) == array.data());
    }

    // Test with two chunks
    {
        std::array array{0, 1, 2, 0, 1, 2};
        ChunkByView view = make_chunk_by_view(array);
        ChunkByIterator it = xranges::next(view.begin(), view.end());

        std::same_as<ChunkByIterator&> decltype(auto) result = --it;
        assert(&result == &it);
        assert(base((*result).begin()) == array.data() + 3);

        --it;
        assert(base((*result).begin()) == array.data());
    }

    // Test going forward and then backward on the same iterator
    {
        std::array array{7, 8, 9, 4, 5, 6, 1, 2, 3, 0};
        ChunkByView view = make_chunk_by_view(array);
        ChunkByIterator it = view.begin();

        ++it;
        --it;
        assert(base((*it).begin()) == array.data());
        assert(base((*it).end()) == array.data() + 3);

        ++it;
        ++it;
        --it;
        assert(base((*it).begin()) == array.data() + 3);
        assert(base((*it).end()) == array.data() + 6);

        ++it;
        ++it;
        --it;
        assert(base((*it).begin()) == array.data() + 6);
        assert(base((*it).end()) == array.data() + 9);

        ++it;
        ++it;
        --it;
        assert(base((*it).begin()) == array.data() + 9);
    }

    // Decrement an iterator multiple times
    if constexpr (xranges::common_range<Underlying>) {
        std::array array{1, 2, 1, 2, 1};
        ChunkByView view = make_chunk_by_view(array);

        ChunkByIterator it = view.end();
        --it;
        --it;
        --it;
        assert(base((*it).begin()) == array.data());
    }

    // Test with a predicate that takes by non-const reference
    if constexpr (!to_underlying(Constant)) {
        std::array array{1, 2, 3, -3, -2, -1};
        View v{Iter{array.data()}, Sent{Iter{array.data() + array.size()}}};
        auto view = xviews::chunk_by(
            std::move(v), [](int& x, int& y) { return x <= y; });

        auto it = xranges::next(view.begin());
        assert(base((*it).begin()) == array.data() + 3);
        --it;
        assert(base((*it).begin()) == array.data());
    }

    // Test with a predicate that is invocable but not callable (i.e. cannot be
    // called like regular function 'f()')
    {
        std::array array = {1, 2, 3, -3, -2, -1};
        auto v =
            View{Iter{array.data()}, Sent{Iter{array.data() + array.size()}}} |
            xviews::transform([](int x) { return IntWrapper{x}; });
        auto view = xviews::chunk_by(std::move(v), &IntWrapper::lessEqual);

        auto it = xranges::next(view.begin());
        assert(base((*it).begin().base()) == array.data() + 3);
        --it;
        assert(base((*it).begin().base()) == array.data());
    }

    // Make sure we do not make a copy of the predicate when we decrement
    if constexpr (xranges::common_range<Underlying>) {
        bool moved = false, copied = false;
        std::array array{1, 2, 1, 3};
        View v{Iter(array.data()), Sent(Iter(array.data() + array.size()))};
        auto view =
            xviews::chunk_by(std::move(v), TrackingPred(&moved, &copied));
        assert(std::exchange(moved, false));
        auto it = view.end();
        --it;
        it--;
        assert(!moved);
        assert(!copied);
    }

    // Check post-decrement
    {
        std::array array{0, 1, 2, -3, -2, -1, -6, -5, -4};
        ChunkByView view = make_chunk_by_view(array);
        ChunkByIterator it = xranges::next(view.begin(), view.end());

        std::same_as<ChunkByIterator> decltype(auto) result = it--;
        assert(result != it);
        assert(result == std::default_sentinel);
        assert(base((*it).begin()) == array.data() + 6);

        result = it--;
        assert(base((*it).begin()) == array.data() + 3);
        assert(base((*result).begin()) == array.data() + 6);

        result = it--;
        assert(base((*it).begin()) == array.data());
        assert(base((*result).begin()) == array.data() + 3);
    }
}

template <class Iterator, IsConst Constant>
constexpr void test_with_pair() {
    // Test with pair of iterators
    test<Iterator, Constant>();

    // Test with iterator-sentinel pair
    test<Iterator, Constant, Iterator>();
}

constexpr bool tests() {
    test_with_pair<bidirectional_iterator<int*>, IsConst::no>();
    test_with_pair<random_access_iterator<int*>, IsConst::no>();
    test_with_pair<contiguous_iterator<int*>, IsConst::no>();
    test_with_pair<int*, IsConst::no>();

    test_with_pair<bidirectional_iterator<int const*>, IsConst::yes>();
    test_with_pair<random_access_iterator<int const*>, IsConst::yes>();
    test_with_pair<contiguous_iterator<int const*>, IsConst::yes>();
    test_with_pair<int const*, IsConst::yes>();

    // Make sure `operator--` isn't provided for non bidirectional ranges
    {
        using ForwardView = View<forward_iterator<int*>,
            sentinel_wrapper<forward_iterator<int*>>>;
        using ChunkByView =
            xranges::chunk_by_view<ForwardView, xranges::less_equal>;
        static_assert(!HasPreDecrement<xranges::iterator_t<ChunkByView>>);
        static_assert(!HasPostDecrement<xranges::iterator_t<ChunkByView>>);
    }

    return true;
}

int main(int, char**) {
    tests();
    static_assert(tests());

    return 0;
}
