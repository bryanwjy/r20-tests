// Copyright 2025-2026 Bryan Wong
// Adapted from GCC testsuite

#include "rxx/optional.h"

#include <cassert>
#include <type_traits>
#include <utility>

struct NonMovable {
    constexpr NonMovable() {}
    NonMovable(NonMovable&&) = delete;
};

struct Tracker {
    int copy = 0;
    int move = 0;

    Tracker() = default;

    constexpr Tracker(Tracker const& o) : copy(o.copy + 1), move(o.move) {}

    constexpr Tracker(Tracker&& o) : copy(o.copy), move(o.move + 1) {}

    Tracker& operator=(Tracker) = delete;

    void reset() { copy = move = 0; }
};

template <typename T>
auto identity_of = []<typename U>(U&& t) -> __RXX optional<T> {
    static_assert(std::is_same_v<T, U&&>);
    assert(t.copy == 0);
    assert(t.move == 0);
    return __RXX optional<T>(t);
};

constexpr void test_and_then() {
    __RXX optional<Tracker> t(std::in_place);
    __RXX optional<Tracker&> rt(t);
    __RXX optional<Tracker const&> rct(t);

    auto r1 = t.and_then(identity_of<Tracker&>);
    assert(r1.has_value());
    assert(&r1.value() == &t.value());

    auto r2 = rt.and_then(identity_of<Tracker&>);
    assert(r2.has_value());
    assert(&r2.value() == &t.value());

    (void)std::as_const(rt).and_then(identity_of<Tracker&>);
    (void)std::move(rt).and_then(identity_of<Tracker&>);

    auto r4 = rct.and_then(identity_of<Tracker const&>);
    assert(r4.has_value());
    assert(&r4.value() == &t.value());

    (void)std::as_const(rct).and_then(identity_of<Tracker const&>);
    (void)std::move(rct).and_then(identity_of<Tracker const&>);

    auto r5 = rt.and_then([](Tracker&) { return __RXX optional<int>(42); });
    static_assert(std::is_same_v<decltype(r5), __RXX optional<int>>);
    assert(r5.has_value());
    assert(r5.value() == 42);

    auto r6 =
        rct.and_then([](Tracker const&) { return __RXX optional<int>(); });
    static_assert(std::is_same_v<decltype(r6), __RXX optional<int>>);
    assert(!r6.has_value());

    rct.reset();
    auto r7 = rct.and_then([](Tracker const&) {
        assert(false);
        return __RXX optional<int>(42);
    });
    static_assert(std::is_same_v<decltype(r7), __RXX optional<int>>);
    assert(!r7.has_value());

    rt.reset();
    auto r8 = rt.and_then([](Tracker&) {
        assert(false);
        return __RXX optional<int>();
    });
    static_assert(std::is_same_v<decltype(r8), __RXX optional<int>>);
    assert(!r8.has_value());
}

template <typename T>
constexpr void test_or_else() {
    T t, u;

    __RXX optional<T&> ot(t);
    auto r1 = ot.or_else([&] {
        assert(false);
        return __RXX optional<T&>(u);
    });
    assert(&ot.value() == &t);
    assert(r1.has_value());
    assert(&r1.value() == &t);
    auto r2 = std::move(ot).or_else([&] {
        assert(false);
        return __RXX optional<T&>();
    });
    assert(&ot.value() == &t);
    assert(r2.has_value());
    assert(&r2.value() == &t);

    ot.reset();
    auto r3 = ot.or_else([&] { return __RXX optional<T&>(u); });
    assert(!ot.has_value());
    assert(r3.has_value());
    assert(&r3.value() == &u);
    auto r4 = std::move(ot).or_else([] { return __RXX optional<T&>(); });
    assert(!ot.has_value());
    assert(!r4.has_value());
}

constexpr void test_transform() {
    __RXX optional<Tracker> t(std::in_place);

    auto r1 = t.transform(&Tracker::copy);
    static_assert(std::is_same_v<decltype(r1), __RXX optional<int&>>);
    assert(r1.has_value());
    assert(&r1.value() == &t->copy);
    auto r2 = std::as_const(t).transform(&Tracker::move);
    static_assert(std::is_same_v<decltype(r2), __RXX optional<int const&>>);
    assert(r2.has_value());
    assert(&r2.value() == &t->move);

    __RXX optional<Tracker&> rt(t);
    auto r3 = rt.transform(&Tracker::copy);
    static_assert(std::is_same_v<decltype(r3), __RXX optional<int&>>);
    assert(r3.has_value());
    assert(&r3.value() == &t->copy);
    auto r4 = std::as_const(rt).transform(&Tracker::copy);
    static_assert(std::is_same_v<decltype(r4), __RXX optional<int&>>);
    assert(r4.has_value());
    assert(&r4.value() == &t->copy);
    auto r5 = std::move(rt).transform(&Tracker::copy);
    static_assert(std::is_same_v<decltype(r5), __RXX optional<int&>>);
    assert(r5.has_value());
    assert(&r5.value() == &t->copy);

    auto r6 = rt.transform([](Tracker& t) { return 10; });
    static_assert(std::is_same_v<decltype(r6), __RXX optional<int>>);
    assert(r6.has_value());
    assert(&r6.value() != &t->copy);
    assert(r6.value() == 10);

    auto r7 = rt.transform([](Tracker& t) { return NonMovable(); });
    static_assert(std::is_same_v<decltype(r7), __RXX optional<NonMovable>>);
    assert(r7.has_value());

    rt.reset();
    auto r8 = rt.transform([](Tracker& t) {
        assert(false);
        return 42;
    });
    static_assert(std::is_same_v<decltype(r8), __RXX optional<int>>);
    assert(!r8.has_value());

    __RXX optional<Tracker const&> crt(t);
    auto r9 = crt.transform(&Tracker::copy);
    static_assert(std::is_same_v<decltype(r9), __RXX optional<int const&>>);
    assert(r9.has_value());
    assert(&r9.value() == &t->copy);
    auto r10 = std::as_const(crt).transform(&Tracker::copy);
    static_assert(std::is_same_v<decltype(r10), __RXX optional<int const&>>);
    assert(r10.has_value());
    assert(&r10.value() == &t->copy);
    auto r11 = std::move(crt).transform(&Tracker::copy);
    static_assert(std::is_same_v<decltype(r11), __RXX optional<int const&>>);
    assert(r11.has_value());
    assert(&r11.value() == &t->copy);

    crt.reset();
    auto r12 = rt.transform([](Tracker& t) {
        assert(false);
        return 42;
    });
    static_assert(std::is_same_v<decltype(r12), __RXX optional<int>>);
    assert(!r12.has_value());
}

int main() {
    auto test_all = [] {
        test_and_then();
        test_transform();
        test_or_else<Tracker>();
        test_or_else<Tracker const>();
        test_or_else<NonMovable>();
        return true;
    };

    test_all();
    static_assert(test_all());
}
