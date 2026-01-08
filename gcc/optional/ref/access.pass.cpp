// Copyright 2025-2026 Bryan Wong
// Adapted from GCC testsuite

#include "rxx/optional.h"

#include <cassert>
#include <type_traits>

struct NonMovable {
    constexpr NonMovable() {}
    NonMovable(NonMovable&&) = delete;
};

template <typename T>
constexpr void test_value(T& t) {
    __RXX optional<T&> o1(t);
    __RXX optional<T&> const co1(t);

    static_assert(std::is_same_v<decltype(o1.value()), T&>);
    assert(&o1.value() == &t);

    static_assert(std::is_same_v<decltype(co1.value()), T&>);
    assert(&co1.value() == &t);

    static_assert(std::is_same_v<decltype(std::move(o1).value()), T&>);
    assert(&std::move(o1).value() == &t);

    static_assert(std::is_same_v<decltype(std::move(co1).value()), T&>);
    assert(&std::move(co1).value() == &t);

    __RXX optional<T const&> o2(t);
    static_assert(std::is_same_v<decltype(o2.value()), T const&>);
    assert(&o2.value() == &t);
}

struct Tracker {
    int copy = 0;
    int move = 0;

    constexpr Tracker(int v) : copy(v), move(v) {}

    constexpr Tracker(Tracker const& o) : copy(o.copy + 1), move(o.move) {}

    constexpr Tracker(Tracker&& o) : copy(o.copy), move(o.move + 1) {}

    Tracker& operator=(Tracker) = delete;
};

constexpr void test_value_or() {
    Tracker t(100), u(200);
    __RXX optional<Tracker&> o(t);

    Tracker r1 = o.value_or(u);
    assert(r1.copy == 101);
    assert(r1.move == 100);
    Tracker r2 = o.value_or(std::move(u));
    assert(r2.copy == 101);
    assert(r2.move == 100);
    Tracker r3 = std::move(o).value_or(u);
    assert(r3.copy == 101);
    assert(r3.move == 100);
    Tracker r4 = std::move(o).value_or(std::move(u));
    assert(r4.copy == 101);
    assert(r4.move == 100);

    o.reset();
    Tracker r5 = o.value_or(u);
    assert(r5.copy == 201);
    assert(r5.move == 200);
    Tracker r6 = o.value_or(std::move(u));
    assert(r6.copy == 200);
    assert(r6.move == 201);
    Tracker r7 = std::move(o).value_or(u);
    assert(r7.copy == 201);
    assert(r7.move == 200);
    Tracker r8 = std::move(o).value_or(std::move(u));
    assert(r8.copy == 200);
    assert(r8.move == 201);
}

template <typename T>
concept has_value_or_for =
    requires(__RXX optional<T&> t, T& u) { t.value_or(u); };

static_assert(has_value_or_for<int>);
static_assert(has_value_or_for<NonMovable>);
static_assert(!has_value_or_for<int[2]>);
static_assert(has_value_or_for<int (*)[2]>);
static_assert(!has_value_or_for<int()>);
static_assert(has_value_or_for<int (*)()>);

int i;
NonMovable nm;
int arr[2];
int foo() {
    return 0;
}

int main() {
    auto test_all = [] {
        test_value<int>(i);
        test_value<NonMovable>(nm);
        test_value<int[2]>(arr);
        test_value<int()>(foo);

        test_value_or();
        return true;
    };

    test_all();
    static_assert(test_all());
}
