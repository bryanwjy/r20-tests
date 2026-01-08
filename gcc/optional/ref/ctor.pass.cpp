// Copyright 2025-2026 Bryan Wong
// Adapted from GCC testsuite

#include "rxx/optional.h"

#include <cassert>
#include <type_traits>

struct NonTrivial {
    constexpr NonTrivial() {}
    constexpr NonTrivial(NonTrivial const&) {};
    constexpr ~NonTrivial() {};
};

struct NonMovable {
    constexpr NonMovable() {}
    NonMovable(NonMovable&&) = delete;
};

template <typename T>
struct Conv {
    T t;

    constexpr operator T() const noexcept { return t; }
};

struct Tracker {
    int copy = 0;
    int move = 0;

    Tracker() = default;

    constexpr Tracker(Tracker const& o) : copy(o.copy + 1), move(o.move) {}

    constexpr Tracker(Tracker&& o) : copy(o.copy), move(o.move + 1) {}

    Tracker& operator=(Tracker) = delete;
};

template <typename T>
constexpr void test_trivial() {
    static_assert(std::is_trivially_copyable_v<__RXX optional<T&>>);
    static_assert(std::is_copy_constructible_v<__RXX optional<T&>>);
    static_assert(std::is_move_constructible_v<__RXX optional<T&>>);
    static_assert(std::is_destructible_v<__RXX optional<T&>>);
}

constexpr void test_trivial_all() {
    test_trivial<int>();
    test_trivial<NonTrivial>();
    test_trivial<NonMovable>();
    test_trivial<__RXX optional<int&>>();
}

constexpr void test_copy() {
    Tracker t;
    __RXX optional<Tracker&> o1(t);
    assert(o1.has_value());
    assert(&o1.value() == &t);
    assert(t.copy == 0);
    assert(t.move == 0);

    __RXX optional<Tracker&> o2(o1);
    assert(o1.has_value());
    assert(&o1.value() == &t);
    assert(o2.has_value());
    assert(&o2.value() == &t);
    assert(t.copy == 0);
    assert(t.move == 0);

    __RXX optional<Tracker&> o3(std::move(o1));
    assert(o1.has_value());
    assert(&o1.value() == &t);
    assert(o3.has_value());
    assert(&o3.value() == &t);
    assert(t.copy == 0);
    assert(t.move == 0);

    __RXX optional<Tracker&> e;
    assert(!e.has_value());

    __RXX optional<Tracker&> o4(e);
    assert(!e.has_value());
    assert(!o4.has_value());

    __RXX optional<Tracker&> o5(std::move(e));
    assert(!e.has_value());
    assert(!o5.has_value());
}

constexpr void test_from_value() {
    NonTrivial v;
    NonTrivial const& cv = v;

    __RXX optional<NonTrivial&> o1(v);
    assert(o1.has_value());
    assert(&o1.value() == &v);
    static_assert(!std::is_constructible_v<__RXX optional<NonTrivial&>,
        NonTrivial const&>);
    static_assert(
        !std::is_constructible_v<__RXX optional<NonTrivial&>, NonTrivial>);
    static_assert(!std::is_constructible_v<__RXX optional<NonTrivial&>,
        NonTrivial const>);

    __RXX optional<NonTrivial&> o2(std::in_place, v);
    assert(o2.has_value());
    assert(&o2.value() == &v);
    static_assert(!std::is_constructible_v<__RXX optional<NonTrivial&>,
        std::in_place_t, NonTrivial const&>);
    static_assert(!std::is_constructible_v<__RXX optional<NonTrivial&>,
        std::in_place_t, NonTrivial>);
    static_assert(!std::is_constructible_v<__RXX optional<NonTrivial&>,
        std::in_place_t, NonTrivial const>);

    __RXX optional<NonTrivial const&> co1(v);
    assert(co1.has_value());
    assert(&co1.value() == &v);
    __RXX optional<NonTrivial const&> co2(cv);
    assert(co2.has_value());
    assert(&co2.value() == &v);
    // No binding to rvalue
    static_assert(!std::is_constructible_v<__RXX optional<NonTrivial const&>,
        NonTrivial>);
    static_assert(!std::is_constructible_v<__RXX optional<NonTrivial const&>,
        NonTrivial const>);

    __RXX optional<NonTrivial const&> co3(std::in_place, v);
    assert(co3.has_value());
    assert(&co3.value() == &v);
    __RXX optional<NonTrivial const&> co4(std::in_place, cv);
    assert(co4.has_value());
    assert(&co4.value() == &v);
    // No binding to rvalue
    static_assert(!std::is_constructible_v<__RXX optional<NonTrivial const&>,
        std::in_place_t, NonTrivial>);
    static_assert(!std::is_constructible_v<__RXX optional<NonTrivial const&>,
        std::in_place_t, NonTrivial const>);

    // Conversion create a pr-value that would bind to temporary
    static_assert(!std::is_constructible_v<__RXX optional<NonTrivial&>,
        Conv<NonTrivial>&>);
    static_assert(!std::is_constructible_v<__RXX optional<NonTrivial&>,
        Conv<NonTrivial> const&>);
    static_assert(!std::is_constructible_v<__RXX optional<NonTrivial&>,
        Conv<NonTrivial>>);
    static_assert(!std::is_constructible_v<__RXX optional<NonTrivial&>,
        Conv<NonTrivial> const>);

    static_assert(!std::is_constructible_v<__RXX optional<NonTrivial&>,
        std::in_place_t, Conv<NonTrivial>&>);
    static_assert(!std::is_constructible_v<__RXX optional<NonTrivial&>,
        std::in_place_t, Conv<NonTrivial> const&>);
    static_assert(!std::is_constructible_v<__RXX optional<NonTrivial&>,
        std::in_place_t, Conv<NonTrivial>>);
    static_assert(!std::is_constructible_v<__RXX optional<NonTrivial&>,
        std::in_place_t, Conv<NonTrivial> const>);

    Conv<NonTrivial&> rw(v);
    Conv<NonTrivial&> const crw(v);

    __RXX optional<NonTrivial&> ro1(rw);
    assert(ro1.has_value());
    assert(&ro1.value() == &v);
    __RXX optional<NonTrivial&> ro2(crw);
    assert(ro2.has_value());
    assert(&ro2.value() == &v);
    __RXX optional<NonTrivial&> ro3(std::move(rw));
    assert(ro3.has_value());
    assert(&ro3.value() == &v);
    __RXX optional<NonTrivial&> ro4(std::move(crw));
    assert(ro4.has_value());
    assert(&ro4.value() == &v);

    __RXX optional<NonTrivial&> ro5(std::in_place, rw);
    assert(ro5.has_value());
    assert(&ro5.value() == &v);
    __RXX optional<NonTrivial&> ro6(std::in_place, crw);
    assert(ro6.has_value());
    assert(&ro6.value() == &v);
    __RXX optional<NonTrivial&> ro7(std::in_place, std::move(rw));
    assert(ro7.has_value());
    assert(&ro7.value() == &v);
    __RXX optional<NonTrivial&> ro8(std::in_place, std::move(crw));
    assert(ro8.has_value());
    assert(&ro8.value() == &v);
}

constexpr void test_from_opt_value() {
    __RXX optional<NonTrivial> v(std::in_place);
    __RXX optional<NonTrivial> const& cv = v;

    __RXX optional<NonTrivial&> o1(v);
    assert(o1.has_value());
    assert(&o1.value() == &v.value());
    static_assert(!std::is_constructible_v<__RXX optional<NonTrivial&>,
        __RXX optional<NonTrivial> const&>);
    static_assert(!std::is_constructible_v<__RXX optional<NonTrivial&>,
        __RXX optional<NonTrivial>>);
    static_assert(!std::is_constructible_v<__RXX optional<NonTrivial&>,
        __RXX optional<NonTrivial> const>);

    __RXX optional<NonTrivial const&> co1(v);
    assert(co1.has_value());
    assert(&co1.value() == &v.value());
    __RXX optional<NonTrivial const&> co2(cv);
    assert(co2.has_value());
    assert(&co2.value() == &v.value());
    // No binding to rvalue
    static_assert(!std::is_constructible_v<__RXX optional<NonTrivial const&>,
        __RXX optional<NonTrivial>>);
    static_assert(!std::is_constructible_v<__RXX optional<NonTrivial const&>,
        __RXX optional<NonTrivial> const>);

    // Conversion create a pr-value that would bind to temporary
    static_assert(!std::is_constructible_v<__RXX optional<NonTrivial&>,
        __RXX optional<Conv<NonTrivial>>&>);
    static_assert(!std::is_constructible_v<__RXX optional<NonTrivial&>,
        __RXX optional<Conv<NonTrivial>> const&>);
    static_assert(!std::is_constructible_v<__RXX optional<NonTrivial&>,
        __RXX optional<Conv<NonTrivial>>>);
    static_assert(!std::is_constructible_v<__RXX optional<NonTrivial&>,
        __RXX optional<Conv<NonTrivial>> const>);

    __RXX optional<Conv<NonTrivial&>> rw(*v);
    __RXX optional<Conv<NonTrivial&> const> crw(*v);

    __RXX optional<NonTrivial&> ro1(rw);
    assert(ro1.has_value());
    assert(&ro1.value() == &v.value());
    __RXX optional<NonTrivial&> ro2(crw);
    assert(ro2.has_value());
    assert(&ro2.value() == &v.value());
    __RXX optional<NonTrivial&> ro3(std::move(rw));
    assert(ro3.has_value());
    assert(&ro3.value() == &v.value());
    __RXX optional<NonTrivial&> ro4(std::move(crw));
    assert(ro4.has_value());
    assert(&ro4.value() == &v.value());
}

constexpr void test_to_opt_value() {
    Tracker t;
    __RXX optional<Tracker&> r(t);
    __RXX optional<Tracker&> const cr(t);

    __RXX optional<Tracker> o1(r);
    assert(o1.has_value());
    assert(o1->copy == 1);
    assert(o1->move == 0);

    __RXX optional<Tracker> o2(cr);
    assert(o2.has_value());
    assert(o2->copy == 1);
    assert(o2->move == 0);

    __RXX optional<Tracker> o3(std::move(r));
    assert(o3.has_value());
    assert(o3->copy == 1);
    assert(o3->move == 0);

    __RXX optional<Tracker> o4(std::move(cr));
    assert(o4.has_value());
    assert(o4->copy == 1);
    assert(o4->move == 0);

    __RXX optional<Tracker&> er;
    __RXX optional<Tracker&> const cer;

    __RXX optional<Tracker> e1(er);
    assert(!e1.has_value());

    __RXX optional<Tracker> e2(cer);
    assert(!e2.has_value());

    __RXX optional<Tracker> e3(std::move(er));
    assert(!e3.has_value());

    __RXX optional<Tracker> e4(std::move(cer));
    assert(!e4.has_value());
}

constexpr void test_opt_opt() {
    __RXX optional<int> s(43);

    __RXX optional<__RXX optional<int>&> o1(s);
    assert(o1.has_value());
    assert(&o1.value() == &s);

    __RXX optional<__RXX optional<int>&> o2(std::in_place, s);
    assert(o2.has_value());
    assert(&o2.value() == &s);

    __RXX optional<__RXX optional<int>> o3(o1);
    assert(o2.has_value());
    assert(o2.value().has_value());
    assert(o2.value() == 43);

    s.reset();
    __RXX optional<__RXX optional<int>&> o4(s);
    assert(o4.has_value());
    assert(&o4.value() == &s);

    __RXX optional<__RXX optional<int>&> o5(std::in_place, s);
    assert(o5.has_value());
    assert(&o5.value() == &s);

    __RXX optional<__RXX optional<int>> o6(o1);
    assert(o6.has_value());
    assert(!o6.value().has_value());

    __RXX optional<__RXX optional<int>> s2(std::in_place);
    __RXX optional<__RXX optional<int>&> oo1(s2);
    assert(oo1.has_value());
    assert(&oo1.value() == &s2.value());

    s2.reset();
    __RXX optional<__RXX optional<int>&> oo2(s2);
    assert(!oo2.has_value());
}

int main() {
    auto test_all = [] {
        test_copy();
        test_from_value();
        test_from_opt_value();
        test_to_opt_value();
        test_opt_opt();
        return true;
    };

    test_all();
    static_assert(test_all());
}
