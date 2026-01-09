// Copyright 2025-2026 Bryan Wong
// Adapted from GCC testsuite

#include "rxx/optional.h"

#include <cassert>

constexpr bool test_and_then() {
    __RXX optional<int> o;
    auto r = o.and_then([](int) -> __RXX optional<short> { throw 1; });
    assert(!r.has_value());
    static_assert(std::is_same_v<decltype(r), __RXX optional<short>>);

    o = 111;
    r = o.and_then([](int i) -> __RXX optional<short> { return {i / 10}; });
    assert(*r == 11);

    return true;
}

static_assert(test_and_then());

enum {
    CalledLvalue = 1,
    CalledConst = 2,
    PassedLvalue = 4,
    PassedConst = 8
};

struct F {
    template <typename This, typename Value>
    static constexpr __RXX optional<int> called_as() {
        int res = 0;
        if constexpr (std::is_lvalue_reference_v<This>)
            res |= CalledLvalue;
        if constexpr (std::is_const_v<std::remove_reference_t<This>>)
            res |= CalledConst;

        if constexpr (std::is_lvalue_reference_v<Value>)
            res |= PassedLvalue;
        if constexpr (std::is_const_v<std::remove_reference_t<Value>>)
            res |= PassedConst;

        return {res};
    }

    template <typename T>
    constexpr __RXX optional<int> operator()(T&&) & {
        return called_as<F&, T>();
    }

    template <typename T>
    constexpr __RXX optional<int> operator()(T&&) const& {
        return called_as<F const&, T>();
    }

    template <typename T>
    constexpr __RXX optional<int> operator()(T&&) && {
        return called_as<F, T>();
    }

    template <typename T>
    constexpr __RXX optional<int> operator()(T&&) const&& {
        return called_as<F const, T>();
    }
};

constexpr bool test_forwarding() {
    __RXX optional<long> o = 1;
    F f;

    assert(*o.and_then(f) == (PassedLvalue | CalledLvalue));
    assert(*o.and_then(std::move(f)) == PassedLvalue);
    assert(*std::move(o).and_then(f) == CalledLvalue);
    assert(*std::move(o).and_then(std::move(f)) == 0);

    auto const& co = o;

    assert(*co.and_then(f) == (PassedLvalue | PassedConst | CalledLvalue));
    assert(*co.and_then(std::move(f)) == (PassedLvalue | PassedConst));
    assert(*std::move(co).and_then(f) == (PassedConst | CalledLvalue));
    assert(*std::move(co).and_then(std::move(f)) == PassedConst);

    auto const& cf = f;

    assert(*o.and_then(cf) == (PassedLvalue | CalledLvalue | CalledConst));
    assert(*o.and_then(std::move(cf)) == (PassedLvalue | CalledConst));
    assert(*std::move(o).and_then(cf) == (CalledLvalue | CalledConst));
    assert(*std::move(o).and_then(std::move(cf)) == CalledConst);

    assert(*co.and_then(cf) ==
        (PassedLvalue | PassedConst | CalledLvalue | CalledConst));
    assert(*co.and_then(std::move(cf)) ==
        (PassedLvalue | PassedConst | CalledConst));
    assert(*std::move(co).and_then(cf) ==
        (PassedConst | CalledLvalue | CalledConst));
    assert(
        *std::move(co).and_then(std::move(cf)) == (PassedConst | CalledConst));

    o = __RXX nullopt;

    assert(!o.and_then(f).has_value());
    assert(!co.and_then(f).has_value());
    assert(!std::move(o).and_then(f).has_value());
    assert(!std::move(co).and_then(f).has_value());

    return true;
}

static_assert(test_forwarding());

void f(int&) {}

void test_unconstrained() {
    // PR libstdc++/102863 - Optional monadic ops should not be constrained
    __RXX optional<int> x;
    auto answer = x.and_then([](auto& y) {
        f(y);
        return __RXX optional<int>{42};
    });
    assert(!answer);
}

int main() {
    test_and_then();
    test_forwarding();
    test_unconstrained();
}
