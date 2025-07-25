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

// constexpr iterator& operator++();
// constexpr iterator operator++(int);

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

struct TrackingPred : TrackInitialization {
    using TrackInitialization::TrackInitialization;
    constexpr bool operator()(int x, int y) const { return x <= y; }
};

template <class Iterator, IsConst Constant>
constexpr void test() {
    using Sentinel = sentinel_wrapper<Iterator>;
    using Underlying = View<Iterator, Sentinel>;
    using ChunkByView = xranges::chunk_by_view<Underlying, xranges::less_equal>;
    using ChunkByIterator = xranges::iterator_t<ChunkByView>;

    auto make_chunk_by_view = [](auto& arr) {
        View view{
            Iterator{arr.data()}, Sentinel{Iterator{arr.data() + arr.size()}}};
        return ChunkByView{std::move(view), xranges::less_equal{}};
    };

    // Increment the iterator when it won't find another satisfied value after
    // begin()
    {
        std::array array{0, 1, 2, 3, 4};
        ChunkByView view = make_chunk_by_view(array);
        ChunkByIterator it = view.begin();

        std::same_as<ChunkByIterator&> decltype(auto) result = ++it;
        assert(&result == &it);
        assert(result == view.end());
        assert(result == std::default_sentinel);
    }

    // Increment the iterator and it finds another value after begin()
    {
        std::array array{1, 2, 3, -1, -2, -3};
        int const* second_chunk = array.data() + 3;
        ChunkByView view = make_chunk_by_view(array);

        ChunkByIterator it = view.begin();
        ++it;
        assert(base((*it).begin()) == second_chunk);
    }

    // Increment advances all the way to the end of the range
    {
        std::array array{1, 2, 3, 4, 1};
        ChunkByView view = make_chunk_by_view(array);

        ChunkByIterator it = view.begin();
        ++it;
        assert(base((*it).begin()) == array.data() + 4);
    }

    // Increment an iterator multiple times
    {
        std::array array{0, 1, 0, 2, 0, 3, 0, 4};
        ChunkByView view = make_chunk_by_view(array);

        ChunkByIterator it = view.begin();
        assert(base((*it).begin()) == array.data());
        ++it;
        assert(base((*it).begin()) == array.data() + 2);
        ++it;
        assert(base((*it).begin()) == array.data() + 4);
        ++it;
        assert(base((*it).begin()) == array.data() + 6);
        ++it;
        assert(it == std::default_sentinel);
    }

    // Test with a predicate that takes by non-const reference
    if constexpr (!to_underlying(Constant)) {
        std::array array{1, 2, 3, -3, -2, -1};
        View v{Iterator{array.data()},
            Sentinel{Iterator{array.data() + array.size()}}};
        auto view = xviews::chunk_by(
            std::move(v), [](int& x, int& y) { return x <= y; });

        auto it = view.begin();
        assert(base((*it).begin()) == array.data());
        ++it;
        assert(base((*it).begin()) == array.data() + 3);
    }

    // Test with a predicate that is invocable but not callable (i.e. cannot be
    // called like regular function 'f()')
    {
        std::array array = {1, 2, 3, -3, -2, -1};
        auto v = View{Iterator{array.data()},
                     Sentinel{Iterator{array.data() + array.size()}}} |
            xviews::transform([](int x) { return IntWrapper{x}; });
        auto view = xviews::chunk_by(std::move(v), &IntWrapper::lessEqual);

        auto it = view.begin();
        assert(base((*it).begin().base()) == array.data());
        ++it;
        assert(base((*it).begin().base()) == array.data() + 3);
    }

    // Make sure we do not make a copy of the predicate when we increment
    // (we should be passing it to ranges::adjacent_find using std::ref)
    {
        bool moved = false, copied = false;
        std::array array{1, 2, 1, 3};
        View v{Iterator(array.data()),
            Sentinel(Iterator(array.data() + array.size()))};
        auto view =
            xviews::chunk_by(std::move(v), TrackingPred(&moved, &copied));
        assert(std::exchange(moved, false));
        auto it = view.begin();
        ++it;
        it++;
        assert(!moved);
        assert(!copied);
    }

    // Check post-increment
    {
        std::array array{0, 1, 2, -3, -2, -1, -6, -5, -4};
        ChunkByView view = make_chunk_by_view(array);
        ChunkByIterator it = view.begin();

        std::same_as<ChunkByIterator> decltype(auto) result = it++;
        assert(result != it);
        assert(base((*result).begin()) == array.data());
        assert(base((*it).begin()) == array.data() + 3);

        result = it++;
        assert(base((*result).begin()) == array.data() + 3);
        assert(base((*it).begin()) == array.data() + 6);

        result = it++;
        assert(base((*result).begin()) == array.data() + 6);
        assert(it == std::default_sentinel);
    }
}

constexpr bool tests() {
    test<forward_iterator<int*>, IsConst::no>();
    test<bidirectional_iterator<int*>, IsConst::no>();
    test<random_access_iterator<int*>, IsConst::no>();
    test<contiguous_iterator<int*>, IsConst::no>();
    test<int*, IsConst::no>();

    test<forward_iterator<int const*>, IsConst::yes>();
    test<bidirectional_iterator<int const*>, IsConst::yes>();
    test<random_access_iterator<int const*>, IsConst::yes>();
    test<contiguous_iterator<int const*>, IsConst::yes>();
    test<int const*, IsConst::yes>();

    return true;
}

int main(int, char**) {
    tests();
    static_assert(tests());

    return 0;
}
