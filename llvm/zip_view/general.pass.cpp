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

// Some basic examples of how zip_view might be used in the wild. This is a
// general collection of sample algorithms and functions that try to mock
// general usage of this view.

#include "rxx/ranges.h"

#include <array>
#include <cassert>
#include <string>
#include <vector>

namespace xranges = rxx::ranges;
namespace xviews = rxx::views;

int main(int, char**) {
    {
        xranges::zip_view v{
            std::array{1, 2},
            std::vector{4, 5, 6},
            std::array{7},
        };
        assert(xranges::size(v) == 1);
        assert(*v.begin() == std::make_tuple(1, 4, 7));
    }
    {
        using namespace std::string_literals;
        std::vector v{1, 2, 3, 4};
        std::array a{"abc"s, "def"s, "gh"s};
        auto view = xviews::zip(v, a);
        auto it = view.begin();
        assert(&(xranges::get_element<0>(*it)) == &(v[0]));
        assert(&(xranges::get_element<1>(*it)) == &(a[0]));

        ++it;
        assert(&(xranges::get_element<0>(*it)) == &(v[1]));
        assert(&(xranges::get_element<1>(*it)) == &(a[1]));

        ++it;
        assert(&(xranges::get_element<0>(*it)) == &(v[2]));
        assert(&(xranges::get_element<1>(*it)) == &(a[2]));

        ++it;
        assert(it == view.end());
    }

    return 0;
}
