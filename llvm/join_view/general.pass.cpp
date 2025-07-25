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

// General tests for join_view. This file does not test anything specifically.

#include "rxx/ranges.h"
#include "types.h"

#include <algorithm>
#include <cassert>
#include <ranges>
#include <string>
#include <vector>

namespace xranges = rxx::ranges;
namespace xviews = rxx::views;

template <class R, class I>
bool isEqual(R& r, I i) {
    for (auto e : r)
        if (e != *i++)
            return false;
    return true;
}

int main(int, char**) {
    {
        int buffer[4][4] = {
            {1111, 2222, 3333, 4444},
            { 555,  666,  777,  888},
            {  99, 1010, 1111, 1212},
            {  13,   14,   15,   16}
        };
        int* flattened = reinterpret_cast<int*>(buffer);

        ChildView children[4] = {ChildView(buffer[0]), ChildView(buffer[1]),
            ChildView(buffer[2]), ChildView(buffer[3])};
        auto jv = xranges::join_view(ParentView(children));
        assert(isEqual(jv, flattened));
    }

    {
        std::vector<std::string> vec = {"Hello", ",", " ", "World", "!"};
        std::string check = "Hello, World!";
        xranges::join_view jv(vec);
        assert(isEqual(jv, check.begin()));
    }

    {
        // P2328R1 join_view should join all views of ranges
        // join a range of prvalue containers
        std::vector x{1, 2, 3, 4};
        auto y = x | xviews::transform([](auto i) {
            std::vector<int> v(i);
            for (int& ii : v) {
                ii = i;
            }
            return v;
        });

        xranges::join_view jv(y);
        std::vector<int> check{1, 2, 2, 3, 3, 3, 4, 4, 4, 4};
        assert(isEqual(jv, check.begin()));
    }

    return 0;
}
