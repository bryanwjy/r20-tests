// Copyright 2025 Bryan Wong
// Adapted from LLVM testsuite

//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "../test_iterators.h"
#include "rxx/ranges/as_rvalue_view.h"
#include "rxx/ranges/subrange.h"
#include "rxx/ranges/view_base.h"

#include <array>
#include <cassert>
#include <concepts>
#include <ranges>

namespace xranges = rxx::ranges;
namespace xviews = rxx::views;

struct SimpleView : xranges::view_base {
    int* begin() const;
    int* end() const;
};

struct NonSimpleView : xranges::view_base {
    char* begin();
    char* end();
    int* begin() const;
    int* end() const;
};

struct NonConstView : xranges::view_base {
    char* begin();
    char* end();
};

template <class T>
concept HasBegin = requires(T t) { t.begin(); };

static_assert(HasBegin<xranges::as_rvalue_view<SimpleView>>);
static_assert(HasBegin<xranges::as_rvalue_view<SimpleView> const>);
static_assert(HasBegin<xranges::as_rvalue_view<NonSimpleView>>);
static_assert(HasBegin<xranges::as_rvalue_view<NonSimpleView> const>);
static_assert(HasBegin<xranges::as_rvalue_view<NonConstView>>);
static_assert(!HasBegin<xranges::as_rvalue_view<NonConstView> const>);

template <class Iter, class Sent>
constexpr void test_range() {
    int a[] = {1, 2};
    xranges::subrange range(Iter(std::begin(a)), Sent(Iter(std::end(a))));
    xranges::as_rvalue_view view(std::move(range));
    std::same_as<std::move_iterator<Iter>> decltype(auto) iter = view.begin();
    assert(base(iter.base()) == std::begin(a));
}

template <class Iter, class Sent>
class WrapRange {
    Iter iter_;
    Sent sent_;

public:
    constexpr WrapRange(Iter iter, Sent sent)
        : iter_(std::move(iter))
        , sent_(std::move(sent)) {}

    constexpr Iter begin() const { return iter_; }
    constexpr Sent end() const { return sent_; }
};

template <class Iter, class Sent>
WrapRange(Iter, Sent) -> WrapRange<Iter, Sent>;

template <class Iter, class Sent>
constexpr void test_const_range() {
    int a[] = {1, 2};
    auto range = WrapRange{Iter(a), Sent(Iter(a + 2))};
    xranges::as_rvalue_view const view(xviews::all(range));
    std::same_as<std::move_iterator<Iter>> decltype(auto) iter = view.begin();
    assert(base(iter.base()) == std::begin(a));
}

struct move_iterator_view : xranges::view_base {
    constexpr std::move_iterator<int*> begin() const { return {}; }
    constexpr std::move_iterator<int*> end() const { return {}; }
};

constexpr bool test() {
    types::for_each(types::cpp20_input_iterator_list<int*>{}, []<class Iter> {
        if constexpr (std::sentinel_for<Iter, Iter>)
            test_range<Iter, Iter>();
        test_range<Iter, sentinel_wrapper<Iter>>();
        test_range<Iter, sized_sentinel<Iter>>();
    });

    types::for_each(types::forward_iterator_list<int const*>{}, []<class Iter> {
        test_const_range<Iter, Iter>();
        test_const_range<Iter, sentinel_wrapper<Iter>>();
        test_const_range<Iter, sized_sentinel<Iter>>();
    });

    { // check that with a std::move_iterator begin() doesn't return
        // move_iterator<move_iterator<T>>
        xranges::as_rvalue_view view{move_iterator_view{}};
        std::same_as<std::move_iterator<int*>> decltype(auto) it = view.begin();
        assert(it == std::move_iterator<int*>{});
    }

    return true;
}

int main(int, char**) {
    test();
    static_assert(test());

    return 0;
}
