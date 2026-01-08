// Copyright 2025-2026 Bryan Wong
// Adapted from GCC testsuite

#include "rxx/optional.h"

#include <cassert>

constexpr bool test_or_else() {
    __RXX optional<int> o;
    auto&& r = o.or_else([]() -> __RXX optional<int> { return {303}; });
    assert(!o);
    assert(*r == 303);
    static_assert(std::is_same_v<decltype(r), __RXX optional<int>&&>);

    o = 808;
    __RXX optional<int> const tr = 909;
    auto&& r2 = o.or_else([&]() -> auto const& { return tr; });
    static_assert(std::is_same_v<decltype(r2), __RXX optional<int>&&>);
    assert(r2 == o);

    return true;
}

static_assert(test_or_else());

constexpr bool test_move() {
    struct X {
        constexpr X() {}
        constexpr X(X const&) { copied = true; }
        constexpr X(X&& x) {
            moved = true;
            x.gone = true;
        }

        bool copied = false;
        bool moved = false;
        bool gone = false;
    };

    __RXX optional<X> o(std::in_place);

    auto f = [] { return __RXX optional<X>{}; };
    assert(o.or_else(f)->copied);
    assert(!o->gone);

    assert(std::move(o).or_else(f)->moved);
    assert(o->gone);

    struct move_only {
        constexpr move_only() {}
        constexpr move_only(move_only&&) {}
    };

    __RXX optional<move_only> mo;
    // doesn't require copy
    (void)std::move(mo).or_else([] { return __RXX optional<move_only>{}; });

    return true;
}

static_assert(test_move());

constexpr bool test_call() {
    struct F {
        constexpr __RXX optional<int> operator()() & { return {1}; }
        constexpr __RXX optional<int> operator()() && { return {2}; }
        constexpr __RXX optional<int> operator()() const& { return {3}; };
        constexpr __RXX optional<int> operator()() const&& { return {4}; }
    };

    __RXX optional<int> o;
    F f;

    assert(*o.or_else(f) == 1);
    assert(*std::move(o).or_else(f) == 1);

    assert(*o.or_else(std::move(f)) == 2);
    assert(*std::move(o).or_else(std::move(f)) == 2);

    F const& cf = f;

    assert(*o.or_else(cf) == 3);
    assert(*std::move(o).or_else(cf) == 3);

    assert(*o.or_else(std::move(cf)) == 4);
    assert(*std::move(o).or_else(std::move(cf)) == 4);

    return true;
}

static_assert(test_call());

int main() {
    test_or_else();
    test_move();
    test_call();
}
