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

// constexpr iterator& operator++();
// constexpr void operator++(int);
// constexpr iterator operator++(int) requires all_forward<Const, Views...>;

#include "../types.h"
#include "rxx/ranges.h"

#include <array>
#include <cassert>

namespace xranges = rxx::ranges;
namespace xviews = rxx::views;

struct InputRange : IntBufferView {
    using IntBufferView::IntBufferView;
    using iterator = cpp20_input_iterator<int*>;
    constexpr iterator begin() const { return iterator(buffer_); }
    constexpr sentinel_wrapper<iterator> end() const {
        return sentinel_wrapper<iterator>(iterator(buffer_ + size_));
    }
};

constexpr bool test() {
    std::array a{1, 2, 3, 4};
    std::array b{4.1, 3.2, 4.3};
    {
        // random/contiguous
        xranges::zip_view v(a, b, xviews::iota(0, 5));
        auto it = v.begin();
        using Iter = decltype(it);

        assert(&(xranges::get_element<0>(*it)) == &(a[0]));
        assert(&(xranges::get_element<1>(*it)) == &(b[0]));
        assert(xranges::get_element<2>(*it) == 0);

        static_assert(std::is_same_v<decltype(++it), Iter&>);

        auto& it_ref = ++it;
        assert(&it_ref == &it);

        assert(&(xranges::get_element<0>(*it)) == &(a[1]));
        assert(&(xranges::get_element<1>(*it)) == &(b[1]));
        assert(xranges::get_element<2>(*it) == 1);

        static_assert(std::is_same_v<decltype(it++), Iter>);
        auto original = it;
        auto copy = it++;
        assert(original == copy);
        assert(&(xranges::get_element<0>(*it)) == &(a[2]));
        assert(&(xranges::get_element<1>(*it)) == &(b[2]));
        assert(xranges::get_element<2>(*it) == 2);
    }

    {
        //  bidi
        int buffer[2] = {1, 2};

        xranges::zip_view v(BidiCommonView{buffer});
        auto it = v.begin();
        using Iter = decltype(it);

        assert(&(xranges::get_element<0>(*it)) == &(buffer[0]));

        static_assert(std::is_same_v<decltype(++it), Iter&>);
        auto& it_ref = ++it;
        assert(&it_ref == &it);
        assert(&(xranges::get_element<0>(*it)) == &(buffer[1]));

        static_assert(std::is_same_v<decltype(it++), Iter>);
        auto original = it;
        auto copy = it++;
        assert(copy == original);
        assert(&(xranges::get_element<0>(*it)) == &(buffer[2]));
    }

    {
        //  forward
        int buffer[2] = {1, 2};

        xranges::zip_view v(ForwardSizedView{buffer});
        auto it = v.begin();
        using Iter = decltype(it);

        assert(&(xranges::get_element<0>(*it)) == &(buffer[0]));

        static_assert(std::is_same_v<decltype(++it), Iter&>);
        auto& it_ref = ++it;
        assert(&it_ref == &it);
        assert(&(xranges::get_element<0>(*it)) == &(buffer[1]));

        static_assert(std::is_same_v<decltype(it++), Iter>);
        auto original = it;
        auto copy = it++;
        assert(copy == original);
        assert(&(xranges::get_element<0>(*it)) == &(buffer[2]));
    }

    {
        // all input+
        int buffer[3] = {4, 5, 6};
        xranges::zip_view v(a, InputRange{buffer});
        auto it = v.begin();
        using Iter = decltype(it);

        assert(&(xranges::get_element<0>(*it)) == &(a[0]));
        assert(&(xranges::get_element<1>(*it)) == &(buffer[0]));

        static_assert(std::is_same_v<decltype(++it), Iter&>);
        auto& it_ref = ++it;
        assert(&it_ref == &it);
        assert(&(xranges::get_element<0>(*it)) == &(a[1]));
        assert(&(xranges::get_element<1>(*it)) == &(buffer[1]));

        static_assert(std::is_same_v<decltype(it++), void>);
        it++;
        assert(&(xranges::get_element<0>(*it)) == &(a[2]));
        assert(&(xranges::get_element<1>(*it)) == &(buffer[2]));
    }

    return true;
}

int main(int, char**) {
    test();
    static_assert(test());

    return 0;
}
