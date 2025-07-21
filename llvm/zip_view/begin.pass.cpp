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

// constexpr auto begin() requires (!(simple-view<Views> && ...));
// constexpr auto begin() const requires (range<const Views> && ...);

#include "rxx/ranges.h"
#include "types.h"

#include <cassert>
#include <concepts>
#include <tuple>
#include <utility>

namespace xranges = rxx::ranges;
namespace xviews = rxx::views;

template <class T>
concept HasConstBegin = requires(T const& ct) { ct.begin(); };

template <class T>
concept HasBegin = requires(T& t) { t.begin(); };

template <class T>
concept HasConstAndNonConstBegin =
    HasConstBegin<T> && requires(T& t, T const& ct) {
        requires !std::same_as<decltype(t.begin()), decltype(ct.begin())>;
    };

template <class T>
concept HasOnlyNonConstBegin = HasBegin<T> && !HasConstBegin<T>;

template <class T>
concept HasOnlyConstBegin = HasConstBegin<T> && !HasConstAndNonConstBegin<T>;

struct NoConstBeginView : xranges::view_base {
    int* begin();
    int* end();
};

constexpr bool test() {
    int buffer[8] = {1, 2, 3, 4, 5, 6, 7, 8};
    {
        // all underlying iterators should be at the begin position
        xranges::zip_view v(SizedRandomAccessView{buffer}, xviews::iota(0),
            xranges::single_view(2.));
        std::same_as<rxx::tuple<int&, int, double&>> decltype(auto) val =
            *v.begin();
        assert(val == std::make_tuple(1, 0, 2.0));
        assert(&(xranges::get_element<0>(val)) == &buffer[0]);
    }

    {
        // with empty range
        xranges::zip_view v(
            SizedRandomAccessView{buffer}, xranges::empty_view<int>());
        assert(v.begin() == v.end());
    }

    {
        // underlying ranges all model simple-view
        xranges::zip_view v(SimpleCommon{buffer}, SimpleCommon{buffer});
        static_assert(std::is_same_v<decltype(v.begin()),
            decltype(std::as_const(v).begin())>);
        assert(v.begin() == std::as_const(v).begin());
        auto [x, y] = *std::as_const(v).begin();
        assert(&x == &buffer[0]);
        assert(&y == &buffer[0]);

        using View = decltype(v);
        static_assert(HasOnlyConstBegin<View>);
        static_assert(!HasOnlyNonConstBegin<View>);
        static_assert(!HasConstAndNonConstBegin<View>);
    }

    {
        // not all underlying ranges model simple-view
        xranges::zip_view v(SimpleCommon{buffer}, NonSimpleNonCommon{buffer});
        static_assert(!std::is_same_v<decltype(v.begin()),
                      decltype(std::as_const(v).begin())>);
        assert(v.begin() == std::as_const(v).begin());
        auto [x, y] = *std::as_const(v).begin();
        assert(&x == &buffer[0]);
        assert(&y == &buffer[0]);

        using View = decltype(v);
        static_assert(!HasOnlyConstBegin<View>);
        static_assert(!HasOnlyNonConstBegin<View>);
        static_assert(HasConstAndNonConstBegin<View>);
    }

    {
        // underlying const R is not a range
        using View = xranges::zip_view<SimpleCommon, NoConstBeginView>;
        static_assert(!HasOnlyConstBegin<View>);
        static_assert(HasOnlyNonConstBegin<View>);
        static_assert(!HasConstAndNonConstBegin<View>);
    }
    return true;
}

int main(int, char**) {
    test();
    static_assert(test());

    return 0;
}
