// Copyright 2025-2026 Bryan Wong
// Adapted from GCC testsuite

// LWG 3886. Monad mo' problems

#include "rxx/optional.h"

void test_cons() {
    struct MoveOnly {
        MoveOnly(int, int) {}
        MoveOnly(MoveOnly&&) {}
    };

    // The {0,0} should be deduced as MoveOnly not const MoveOnly
    [[maybe_unused]] __RXX optional<MoveOnly const> o({0, 0});
}

struct Tracker {
    bool moved = false;
    constexpr Tracker(int, int) {}
    constexpr Tracker(Tracker const&) {}
    constexpr Tracker(Tracker&&) : moved(true) {}

    // The follow means that is_assignable<const Tracker&, U> is true:
    template <typename T>
    constexpr void operator=(T&&) const {}
};

void test_assignment() {
    constexpr bool moved = [] {
        __RXX optional<Tracker const> o;
        // The {0,0} should be deduced as Tracker not const Tracker:
        o = {0, 0};
        // So the contained value should have been move constructed not copied:
        return o->moved;
    }();
    static_assert(moved);
}

void test_value_or() {
    constexpr bool moved = [] {
        __RXX optional<Tracker const> o;
        return o.value_or({0, 0}).moved;
    }();
    static_assert(moved);

    constexpr bool moved_rval = [] {
        __RXX optional<Tracker const> o;
        return std::move(o).value_or({0, 0}).moved;
    }();
    static_assert(moved_rval);
}
