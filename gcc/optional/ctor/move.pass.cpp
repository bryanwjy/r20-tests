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

struct throwing_move {
    throwing_move() = default;
    throwing_move(throwing_move const&) { throw exception{}; }
};

int main() {
    // [20.5.4.1] Constructors

    {
        __RXX optional<long> o;
        auto moved_to = std::move(o);
        assert(!moved_to);
        assert(!o);
    }

    {
        long const val = 0x1234ABCD;
        __RXX optional<long> o{std::in_place, val};
        auto moved_to = std::move(o);
        assert(moved_to);
        assert(*moved_to == val);
        assert(o && *o == val);
    }

    {
        __RXX optional<tracker> o;
        auto moved_to = std::move(o);
        assert(!moved_to);
        assert(tracker::count == 0);
        assert(!o);
    }

    {
        __RXX optional<tracker> o{std::in_place, 333};
        auto moved_to = std::move(o);
        assert(moved_to);
        assert(moved_to->value == 333);
        assert(tracker::count == 2);
        assert(o && o->value == -1);
    }

    enum outcome {
        nothrow,
        caught,
        bad_catch
    };

    {
        outcome result = nothrow;
        __RXX optional<throwing_move> o;

        try {
            auto moved_to = std::move(o);
        } catch (exception const&) {
            result = caught;
        } catch (...) {
            result = bad_catch;
        }

        assert(result == nothrow);
    }

    {
        outcome result = nothrow;
        __RXX optional<throwing_move> o{std::in_place};

        try {
            auto moved_to = std::move(o);
        } catch (exception const&) {
            result = caught;
        } catch (...) {
            result = bad_catch;
        }

        assert(result == caught);
    }

    assert(tracker::count == 0);
}
