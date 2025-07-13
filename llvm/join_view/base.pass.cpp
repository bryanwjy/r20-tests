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

// constexpr V base() const& requires copy_constructible<V>;
// constexpr V base() &&;

#include "../static_asserts.h"
#include "rxx/join_view.h"
#include "types.h"

#include <cassert>
#include <ranges>

namespace xranges = rxx::ranges;
namespace xviews = rxx::views;
constexpr bool hasLValueQualifiedBase(auto&& view) {
    return requires { view.base(); };
}

constexpr bool test() {
    int buffer[4][4] = {
        {1111, 2222, 3333, 4444},
        { 555,  666,  777,  888},
        {  99, 1010, 1111, 1212},
        {  13,   14,   15,   16}
    };

    {
        ChildView children[4] = {ChildView(buffer[0]), ChildView(buffer[1]),
            ChildView(buffer[2]), ChildView(buffer[3])};
        auto jv = xranges::join_view(ParentView{children});
        assert(std::move(jv).base().ptr_ == children);

        static_assert(!hasLValueQualifiedBase(jv));
        ASSERT_SAME_TYPE(decltype(std::move(jv).base()), ParentView<ChildView>);
    }

    {
        xranges::join_view jv(buffer);
        assert(jv.base().base() == buffer + 0);

        static_assert(hasLValueQualifiedBase(jv));
        ASSERT_SAME_TYPE(decltype(jv.base()), std::ranges::ref_view<int[4][4]>);
    }

    {
        xranges::join_view const jv(buffer);
        assert(jv.base().base() == buffer + 0);

        static_assert(hasLValueQualifiedBase(jv));
        ASSERT_SAME_TYPE(decltype(jv.base()), std::ranges::ref_view<int[4][4]>);
    }

    return true;
}

int main(int, char**) {
    test();
    static_assert(test());

    return 0;
}
