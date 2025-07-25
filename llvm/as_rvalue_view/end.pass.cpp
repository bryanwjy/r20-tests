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
#include <ranges>

namespace xranges = rxx::ranges;
namespace xviews = rxx::views;

struct DefaultConstructibleView : xranges::view_base {
    int* begin() const;
    int* end() const;
};

struct CVCallView : xranges::view_base {
    mutable bool const_called = false;
    mutable int i[1];
    constexpr int* begin() {
        const_called = false;
        return i;
    }

    constexpr int* begin() const {
        const_called = true;
        return i;
    }

    constexpr int* end() {
        const_called = false;
        return i + 1;
    }

    constexpr int* end() const {
        const_called = true;
        return i + 1;
    }
};

struct NonConstCommonRange : xranges::view_base {
    int* begin();
    int* end();

    int* begin() const;
    sentinel_wrapper<int*> end() const;
};

struct NonConstView : xranges::view_base {
    int* begin();
    int* end();
};

template <class T>
concept HasEnd = requires(T t) { t.end(); };

static_assert(HasEnd<xranges::as_rvalue_view<DefaultConstructibleView>>);
static_assert(HasEnd<xranges::as_rvalue_view<DefaultConstructibleView> const>);
static_assert(HasEnd<xranges::as_rvalue_view<NonConstView>>);
static_assert(!HasEnd<xranges::as_rvalue_view<NonConstView> const>);

static_assert(std::is_same_v<
    decltype(std::declval<xranges::as_rvalue_view<DefaultConstructibleView>>()
                 .end()),
    __RXX move_iterator<int*>>);
static_assert(std::is_same_v<
    decltype(std::declval<xranges::as_rvalue_view<NonConstCommonRange> const>()
                 .end()),
    __RXX move_sentinel<sentinel_wrapper<int*>>>);

template <class Iter, class Sent, bool is_common>
constexpr void test_range() {
    using Expected = std::conditional_t<is_common, __RXX move_iterator<Sent>,
        __RXX move_sentinel<Sent>>;
    int a[] = {1, 2};
    xranges::subrange range(Iter(std::begin(a)), Sent(Iter(std::end(a))));
    xranges::as_rvalue_view view(std::move(range));
    std::same_as<Expected> decltype(auto) iter = view.end();
    assert(base(base(iter.base())) == std::end(a));
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

template <class Iter, class Sent, bool is_common>
constexpr void test_const_range() {
    using Expected = std::conditional_t<is_common, __RXX move_iterator<Sent>,
        __RXX move_sentinel<Sent>>;
    int a[] = {1, 2};
    auto range = WrapRange{Iter(a), Sent(Iter(a + 2))};
    xranges::as_rvalue_view const view(std::move(range));
    std::same_as<Expected> decltype(auto) iter = view.end();
    assert(base(base(iter.base())) == std::end(a));
}

struct move_iterator_view : xranges::view_base {
    constexpr __RXX move_iterator<int*> begin() const { return {}; }
    constexpr __RXX move_iterator<int*> end() const { return {}; }
};

constexpr bool test() {
    test_range<cpp17_input_iterator<int*>,
        sentinel_wrapper<cpp17_input_iterator<int*>>, false>();
    test_range<cpp17_input_iterator<int*>,
        sized_sentinel<cpp17_input_iterator<int*>>, false>();
    test_range<cpp20_input_iterator<int*>,
        sentinel_wrapper<cpp20_input_iterator<int*>>, false>();
    test_range<cpp20_input_iterator<int*>,
        sized_sentinel<cpp20_input_iterator<int*>>, false>();

    types::for_each(types::forward_iterator_list<int*>{}, []<class Iter> {
        test_range<Iter, Iter, true>();
        test_range<Iter, sentinel_wrapper<Iter>, false>();
        test_range<Iter, sized_sentinel<Iter>, false>();
    });

    {
        xranges::as_rvalue_view view(CVCallView{});
        (void)view.end();
        assert(view.base().const_called);
    }

    { // check that with a __RXX move_iterator begin() doesn't return
        // move_iterator<move_iterator<T>>
        xranges::as_rvalue_view view{move_iterator_view{}};
        std::same_as<__RXX move_iterator<int*>> decltype(auto) it =
            view.end();
        assert(it == __RXX move_iterator<int*>{});
    }

    return true;
}

int main(int, char**) {
    test();

// gcc cannot have mutable member in constant expression
#if !RXX_COMPILER_GCC
    static_assert(test());
#endif

    return 0;
}
