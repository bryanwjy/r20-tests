// Copyright 2025-2026 Bryan Wong
// Adapted from GCC testsuite

#include "rxx/optional.h"

#if RXX_SUPPORTS_OPTIONAL_REFERENCES
#  include <cassert>
#  include <type_traits>

struct NonTrivial {
    constexpr NonTrivial() {}
    constexpr NonTrivial(NonTrivial const&) {};
    constexpr ~NonTrivial() {};
};

struct NonMovable {
    NonMovable() = default;
    NonMovable(NonMovable&&) = delete;
};

template <typename T>
struct Conv {
    T t;

    constexpr operator T() const noexcept { return t; }
};

struct Tracker {
    struct Counter {
        int copy;
        int move;
    };

    Counter ctor{0, 0};
    Counter assign{0, 0};

    Tracker() = default;

    constexpr Tracker(Tracker const& o) : ctor(o.ctor), assign(o.assign) {
        ctor.copy += 1;
    }

    constexpr Tracker(Tracker&& o) : ctor(o.ctor), assign(o.assign) {
        ctor.move += 1;
    }

    constexpr Tracker& operator=(Tracker const& o) {
        assign.copy += 1;
        return *this;
    }

    constexpr Tracker& operator=(Tracker&& o) {
        assign.move += 1;
        return *this;
    }
};

template <typename T>
constexpr void test_trivial() {
    static_assert(std::is_copy_assignable_v<__RXX optional<T&>>);
    static_assert(std::is_move_assignable_v<__RXX optional<T&>>);
}

constexpr void test_trivial_all() {
    test_trivial<int>();
    test_trivial<NonTrivial>();
    test_trivial<__RXX optional<int&>>();
}

constexpr void test_copy() {
    Tracker t, u;
    __RXX optional<Tracker&> e;
    __RXX optional<Tracker&> o1(t);
    __RXX optional<Tracker&> o2(u);

    o2 = o1;
    assert(o1.has_value());
    assert(&o1.value() == &t);
    assert(o2.has_value());
    assert(&o2.value() == &t);
    assert(t.ctor.copy == 0);
    assert(t.ctor.move == 0);
    assert(t.assign.copy == 0);
    assert(t.assign.move == 0);

    o2 = e;
    assert(!o2.has_value());

    o2 = std::move(o1);
    assert(o1.has_value());
    assert(&o1.value() == &t);
    assert(o2.has_value());
    assert(&o2.value() == &t);
    assert(t.ctor.copy == 0);
    assert(t.ctor.move == 0);
    assert(t.assign.copy == 0);
    assert(t.assign.move == 0);

    o2 = std::move(e);
    assert(!o2.has_value());
}

template <typename T, typename U>
concept can_emplace = requires(T t, U&& u) { t.emplace(std::forward<U>(u)); };

constexpr void test_from_value() {
    NonTrivial v, u;
    NonTrivial const& cv = v;
    __RXX optional<NonTrivial&> const s(u);
    __RXX optional<NonTrivial&> o1;
    __RXX optional<NonTrivial const&> co1;

    o1 = s;
    o1 = v;
    assert(o1.has_value());
    assert(&o1.value() == &v);

    o1.reset();
    assert(!o1.has_value());

    o1 = v;
    assert(o1.has_value());
    assert(&o1.value() == &v);
    static_assert(!std::is_assignable_v<__RXX optional<NonTrivial&>,
        NonTrivial const&>);
    static_assert(
        !std::is_assignable_v<__RXX optional<NonTrivial&>, NonTrivial>);
    static_assert(
        !std::is_assignable_v<__RXX optional<NonTrivial&>, NonTrivial const>);

    o1 = s;
    o1.emplace(v);
    assert(o1.has_value());
    assert(&o1.value() == &v);

    o1 = __RXX nullopt;
    assert(!o1.has_value());

    o1.emplace(v);
    assert(o1.has_value());
    assert(&o1.value() == &v);

    static_assert(
        !can_emplace<__RXX optional<NonTrivial&>, NonTrivial const&>);
    static_assert(!can_emplace<__RXX optional<NonTrivial&>, NonTrivial>);
    static_assert(
        !can_emplace<__RXX optional<NonTrivial&>, NonTrivial const>);

    co1 = s;
    co1 = v;
    assert(co1.has_value());
    assert(&co1.value() == &v);

    co1 = __RXX nullopt;
    co1 = cv;
    assert(co1.has_value());
    assert(&co1.value() == &v);
    // No binding to rvalue
    static_assert(
        !std::is_assignable_v<__RXX optional<NonTrivial const&>, NonTrivial>);
    static_assert(!std::is_assignable_v<__RXX optional<NonTrivial const&>,
        NonTrivial const>);

    co1 = __RXX nullopt;
    co1.emplace(v);
    assert(co1.has_value());
    assert(&co1.value() == &v);

    co1 = s;
    co1.emplace(cv);
    assert(co1.has_value());
    assert(&co1.value() == &v);
    // No binding to rvalue
    static_assert(
        !can_emplace<__RXX optional<NonTrivial const&>, NonTrivial const>);
    static_assert(
        !can_emplace<__RXX optional<NonTrivial const&>, NonTrivial>);

    // Conversion create a pr-value that would bind to temporary
    static_assert(!std::is_assignable_v<__RXX optional<NonTrivial&>,
        Conv<NonTrivial>&>);
    static_assert(!std::is_assignable_v<__RXX optional<NonTrivial&>,
        Conv<NonTrivial> const&>);
    static_assert(
        !std::is_assignable_v<__RXX optional<NonTrivial&>, Conv<NonTrivial>>);
    static_assert(!std::is_assignable_v<__RXX optional<NonTrivial&>,
        Conv<NonTrivial> const>);

    static_assert(
        !can_emplace<__RXX optional<NonTrivial&>, Conv<NonTrivial>&>);
    static_assert(
        !can_emplace<__RXX optional<NonTrivial&>, Conv<NonTrivial> const&>);
    static_assert(
        !can_emplace<__RXX optional<NonTrivial&>, Conv<NonTrivial>>);
    static_assert(
        !can_emplace<__RXX optional<NonTrivial&>, Conv<NonTrivial> const>);

    Conv<NonTrivial&> rw(v);
    Conv<NonTrivial&> const crw(v);

    o1 = __RXX nullopt;
    o1 = rw;
    assert(o1.has_value());
    assert(&o1.value() == &v);
    o1 = s;
    o1 = crw;
    assert(o1.has_value());
    assert(&o1.value() == &v);
    o1 = s;
    o1 = std::move(rw);
    assert(o1.has_value());
    assert(&o1.value() == &v);
    o1 = __RXX nullopt;
    o1 = std::move(crw);
    assert(o1.has_value());
    assert(&o1.value() == &v);

    o1 = s;
    o1.emplace(rw);
    assert(o1.has_value());
    assert(&o1.value() == &v);
    o1 = __RXX nullopt;
    o1.emplace(crw);
    assert(o1.has_value());
    assert(&o1.value() == &v);
    o1 = __RXX nullopt;
    o1.emplace(std::move(rw));
    assert(o1.has_value());
    assert(&o1.value() == &v);
    o1 = s;
    o1.emplace(std::move(crw));
    assert(o1.has_value());
    assert(&o1.value() == &v);
}

constexpr void test_from_opt_value() {
    NonTrivial u;
    __RXX optional<NonTrivial> v(std::in_place);
    __RXX optional<NonTrivial> const& cv = v;

    __RXX optional<NonTrivial&> const s(u);
    __RXX optional<NonTrivial&> o1;
    __RXX optional<NonTrivial const&> co1;

    o1 = s;
    o1 = v;
    assert(o1.has_value());
    assert(&o1.value() == &v.value());
    o1 = __RXX nullopt;
    o1 = v;
    assert(o1.has_value());
    assert(&o1.value() == &v.value());
    static_assert(!std::is_assignable_v<__RXX optional<NonTrivial&>,
        __RXX optional<NonTrivial> const&>);
    static_assert(!std::is_assignable_v<__RXX optional<NonTrivial&>,
        __RXX optional<NonTrivial>>);
    static_assert(!std::is_assignable_v<__RXX optional<NonTrivial&>,
        __RXX optional<NonTrivial> const>);

    co1 = s;
    co1 = v;
    assert(co1.has_value());
    assert(&co1.value() == &v.value());
    co1 = __RXX nullopt;
    co1 = cv;
    assert(co1.has_value());
    assert(&co1.value() == &v.value());
    // No binding to rvalue
    static_assert(!std::is_assignable_v<__RXX optional<NonTrivial const&>,
        __RXX optional<NonTrivial>>);
    static_assert(!std::is_assignable_v<__RXX optional<NonTrivial const&>,
        __RXX optional<NonTrivial> const>);

    // Conversion create a pr-value that would bind to temporary
    static_assert(!std::is_assignable_v<__RXX optional<NonTrivial&>,
        __RXX optional<Conv<NonTrivial>>&>);
    static_assert(!std::is_assignable_v<__RXX optional<NonTrivial&>,
        __RXX optional<Conv<NonTrivial>> const&>);
    static_assert(!std::is_assignable_v<__RXX optional<NonTrivial&>,
        __RXX optional<Conv<NonTrivial>>>);
    static_assert(!std::is_assignable_v<__RXX optional<NonTrivial&>,
        __RXX optional<Conv<NonTrivial>> const>);

    __RXX optional<Conv<NonTrivial&>> rw(*v);
    __RXX optional<Conv<NonTrivial&> const> crw(*v);

    o1 = __RXX nullopt;
    o1 = rw;
    assert(o1.has_value());
    assert(&o1.value() == &v.value());
    o1 = s;
    o1 = crw;
    assert(o1.has_value());
    assert(&o1.value() == &v.value());
    o1 = s;
    o1 = std::move(rw);
    assert(o1.has_value());
    assert(&o1.value() == &v.value());
    o1 = __RXX nullopt;
    o1 = std::move(crw);
    assert(o1.has_value());
    assert(&o1.value() == &v.value());
}

constexpr void test_to_opt_value() {
    Tracker t;
    __RXX optional<Tracker&> er;
    __RXX optional<Tracker&> r(t);
    __RXX optional<Tracker&> const cr(t);

    __RXX optional<Tracker> o1;
    o1 = r;
    assert(o1.has_value());
    assert(o1->ctor.copy == 1);
    assert(o1->ctor.move == 0);
    assert(o1->assign.copy == 0);
    assert(o1->assign.move == 0);

    o1 = r;
    assert(o1.has_value());
    assert(o1->ctor.copy == 1);
    assert(o1->ctor.move == 0);
    assert(o1->assign.copy == 1);
    assert(o1->assign.move == 0);

    o1 = er;
    assert(!o1.has_value());

    o1 = cr;
    assert(o1.has_value());
    assert(o1->ctor.copy == 1);
    assert(o1->ctor.move == 0);
    assert(o1->assign.copy == 0);
    assert(o1->assign.move == 0);

    o1 = cr;
    assert(o1.has_value());
    assert(o1->ctor.copy == 1);
    assert(o1->ctor.move == 0);
    assert(o1->assign.copy == 1);
    assert(o1->assign.move == 0);

    o1 = std::move(er);

    o1 = std::move(r);
    assert(o1.has_value());
    assert(o1->ctor.copy == 1);
    assert(o1->ctor.move == 0);
    assert(o1->assign.copy == 0);
    assert(o1->assign.move == 0);

    o1 = std::move(cr);
    assert(o1.has_value());
    assert(o1->ctor.copy == 1);
    assert(o1->ctor.move == 0);
    assert(o1->assign.copy == 1);
    assert(o1->assign.move == 0);
}

constexpr void test_swap() {
    NonMovable a, b;
    __RXX optional<NonMovable&> oa(a), ob(b);

    oa.swap(ob);
    static_assert(noexcept(oa.swap(ob)));
    assert(oa.has_value());
    assert(&oa.value() == &b);
    assert(ob.has_value());
    assert(&ob.value() == &a);

    swap(oa, ob);
    static_assert(std::is_nothrow_swappable_v<__RXX optional<NonMovable&>>);
    assert(oa.has_value());
    assert(&oa.value() == &a);
    assert(ob.has_value());
    assert(&ob.value() == &b);

    ob.reset();
    oa.swap(ob);
    assert(!oa.has_value());
    assert(ob.has_value());
    assert(&ob.value() == &a);

    ob.reset();
    std::swap(oa, ob);
    assert(!oa.has_value());
    assert(!ob.has_value());

    __RXX optional<NonMovable const&> ca(a), cb(b);
    swap(ca, cb);
    assert(ca.has_value());
    assert(&ca.value() == &b);
    assert(cb.has_value());
    assert(&cb.value() == &a);

    static_assert(!std::is_swappable_with_v<__RXX optional<int>&,
        __RXX optional<int&>&>);
}

int main() {
    auto test_all = [] {
        test_copy();
        test_from_value();
        test_from_opt_value();
        test_to_opt_value();
        test_swap();
        return true;
    };

    test_all();
    static_assert(test_all());
}
#else
int main() {}
#endif
