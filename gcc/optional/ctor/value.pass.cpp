// Copyright 2025-2026 Bryan Wong
// Adapted from GCC testsuite

// Copyright (C) 2013-2026 Free Software Foundation, Inc.
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

#include "rxx/optional.h"

#include <cassert>
#include <string>
#include <vector>

struct tracker {
    tracker(int value) : value(value) { ++count; }
    ~tracker() { --count; }

    tracker(tracker const& other) : value(other.value) { ++count; }
    tracker(tracker&& other) : value(other.value) {
        other.value = -1;
        ++count;
    }

    tracker& operator=(tracker const&) = default;
    tracker& operator=(tracker&&) = default;

    int value;

    static int count;
};

int tracker::count = 0;

struct exception {};

struct throwing_construction {
    explicit throwing_construction(bool propagate) : propagate(propagate) {}

    throwing_construction(throwing_construction const& other)
        : propagate(other.propagate) {
        if (propagate)
            throw exception{};
    }

    bool propagate;
};

int main() {
    // [20.5.4.1] Constructors

    {
        auto i = 0x1234ABCD;
        __RXX optional<long> o{i};
        assert(o);
        assert(*o == 0x1234ABCD);
        assert(i == 0x1234ABCD);
    }

    {
        auto i = 0x1234ABCD;
        __RXX optional<long> o = i;
        assert(o);
        assert(*o == 0x1234ABCD);
        assert(i == 0x1234ABCD);
    }

    {
        auto i = 0x1234ABCD;
        __RXX optional<long> o = {i};
        assert(o);
        assert(*o == 0x1234ABCD);
        assert(i == 0x1234ABCD);
    }

    {
        auto i = 0x1234ABCD;
        __RXX optional<long> o{std::move(i)};
        assert(o);
        assert(*o == 0x1234ABCD);
        assert(i == 0x1234ABCD);
    }

    {
        auto i = 0x1234ABCD;
        __RXX optional<long> o = std::move(i);
        assert(o);
        assert(*o == 0x1234ABCD);
        assert(i == 0x1234ABCD);
    }

    {
        auto i = 0x1234ABCD;
        __RXX optional<long> o = {std::move(i)};
        assert(o);
        assert(*o == 0x1234ABCD);
        assert(i == 0x1234ABCD);
    }

    {
        std::vector<int> v = {0, 1, 2, 3, 4, 5};
        __RXX optional<std::vector<int>> o{v};
        assert(!v.empty());
        assert(o->size() == 6);
    }

    {
        std::vector<int> v = {0, 1, 2, 3, 4, 5};
        __RXX optional<std::vector<int>> o = v;
        assert(!v.empty());
        assert(o->size() == 6);
    }

    {
        std::vector<int> v = {0, 1, 2, 3, 4, 5};
        __RXX optional<std::vector<int>> o{v};
        assert(!v.empty());
        assert(o->size() == 6);
    }

    {
        std::vector<int> v = {0, 1, 2, 3, 4, 5};
        __RXX optional<std::vector<int>> o{std::move(v)};
        assert(v.empty());
        assert(o->size() == 6);
    }

    {
        std::vector<int> v = {0, 1, 2, 3, 4, 5};
        __RXX optional<std::vector<int>> o = std::move(v);
        assert(v.empty());
        assert(o->size() == 6);
    }

    {
        std::vector<int> v = {0, 1, 2, 3, 4, 5};
        __RXX optional<std::vector<int>> o{std::move(v)};
        assert(v.empty());
        assert(o->size() == 6);
    }

    {
        tracker t{333};
        __RXX optional<tracker> o = t;
        assert(o->value == 333);
        assert(tracker::count == 2);
        assert(t.value == 333);
    }

    {
        tracker t{333};
        __RXX optional<tracker> o = std::move(t);
        assert(o->value == 333);
        assert(tracker::count == 2);
        assert(t.value == -1);
    }

    enum outcome {
        nothrow,
        caught,
        bad_catch
    };

    {
        outcome result = nothrow;
        throwing_construction t{false};

        try {
            __RXX optional<throwing_construction> o{t};
        } catch (exception const&) {
            result = caught;
        } catch (...) {
            result = bad_catch;
        }

        assert(result == nothrow);
    }

    {
        outcome result = nothrow;
        throwing_construction t{true};

        try {
            __RXX optional<throwing_construction> o{t};
        } catch (exception const&) {
            result = caught;
        } catch (...) {
            result = bad_catch;
        }

        assert(result == caught);
    }

    {
        outcome result = nothrow;
        throwing_construction t{false};

        try {
            __RXX optional<throwing_construction> o{std::move(t)};
        } catch (exception const&) {
            result = caught;
        } catch (...) {
            result = bad_catch;
        }

        assert(result == nothrow);
    }

    {
        outcome result = nothrow;
        throwing_construction t{true};

        try {
            __RXX optional<throwing_construction> o{std::move(t)};
        } catch (exception const&) {
            result = caught;
        } catch (...) {
            result = bad_catch;
        }

        assert(result == caught);
    }

    {
        __RXX optional<std::string> os = "foo";
        struct X {
            explicit X(int) {}
            X& operator=(int) { return *this; }
        };
        __RXX optional<X> ox{42};
        __RXX optional<int> oi{42};
        __RXX optional<X> ox2{oi};
        __RXX optional<std::string> os2;
        os2 = "foo";
        __RXX optional<X> ox3;
        ox3 = 42;
        __RXX optional<X> ox4;
        ox4 = oi;
    }
    {
        __RXX optional<int> oi = __RXX optional<short>();
        assert(!bool(oi));
        __RXX optional<std::string> os = __RXX optional<char const*>();
        assert(!bool(os));
        __RXX optional<__RXX optional<int>> ooi = __RXX optional<int>();
        assert(bool(ooi));
        ooi = __RXX optional<int>();
        assert(bool(ooi));
        ooi = __RXX optional<int>(42);
        assert(bool(ooi));
        assert(bool(*ooi));
        __RXX optional<__RXX optional<int>> ooi2 = __RXX optional<short>();
        assert(bool(ooi2));
        ooi2 = __RXX optional<short>();
        assert(bool(ooi2));
        ooi2 = __RXX optional<short>(6);
        assert(bool(ooi2));
        assert(bool(*ooi2));
        __RXX optional<__RXX optional<int>> ooi3 = __RXX optional<int>(42);
        assert(bool(ooi3));
        assert(bool(*ooi3));
        __RXX optional<__RXX optional<int>> ooi4 = __RXX optional<short>(6);
        assert(bool(ooi4));
        assert(bool(*ooi4));
    }
}
