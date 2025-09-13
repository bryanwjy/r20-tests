// Copyright 2025 Bryan Wong
// Adapted from GCC testsuite

// Copyright (C) 2023-2025 Free Software Foundation, Inc.
//
// This file is part of the GNU ISO C++ Library.  This library is free
// software; you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the
// Free Software Foundation; either version 3, or (at your option)
// any later version.

// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along
// with this library; see the file COPYING3.  If not see
// <http://www.gnu.org/licenses/>.

#include "rxx/config.h"

#include "rxx/generator.h"

template <typename... Ts>
using xgenerator = rxx::generator<Ts...>;

template <unsigned MaxCopies>
struct copy_max {
    int copy = 0;

    copy_max() {}

    copy_max(copy_max const& o) : copy{o.copy + 1} {
        assert(copy <= MaxCopies);
    }

    copy_max& operator=(copy_max const& o) {
        copy = o.copy + 1;
        assert(copy <= MaxCopies);
        return *this;
    }

    copy_max(copy_max&& o) { std::swap(o.copy, this->copy); }

    copy_max& operator=(copy_max&& o) {
        std::swap(o.copy, this->copy);
        return *this;
    }
};

template <typename Ref>
xgenerator<Ref> foo() {
    co_yield {};
}

int main() {
    static_assert(!std::copy_constructible<xgenerator<int>>);
    {
        auto gen = foo<copy_max<1> const&>();
        auto i = gen.begin();
        *i;
        *i;
        auto is = *i;
        assert(is.copy > 0);
    }

    {
        auto gen2 = foo<copy_max<0>&&>();
        auto i = gen2.begin();
        *i;
        *i;
        auto is = *i;
    }

    {
        auto gen = foo<copy_max<0>>(); // should be same as case 2
        auto i = gen.begin();
        *i;
        *i;
        auto is = *i;
    }
}
