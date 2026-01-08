// Copyright 2025-2026 Bryan Wong
// Adapted from GCC testsuite

#include "rxx/optional.h"

#include <cassert>

constexpr bool test_transform() {
    __RXX optional<int> o;
    auto&& r = o.transform([](int) -> unsigned { throw 1; });
    static_assert(std::is_same_v<decltype(r), __RXX optional<unsigned>&&>);
    assert(!r.has_value());

    o = 10;
    auto&& r2 = o.transform([](int i) -> unsigned { return i + 2u; });
    static_assert(std::is_same_v<decltype(r2), __RXX optional<unsigned>&&>);
    assert(*r2 == 12u);

    return true;
}

static_assert(test_transform());

enum {
    CalledLvalue = 1,
    CalledConst = 2,
    PassedLvalue = 4,
    PassedConst = 8
};

struct F {
    template <typename This, typename Value>
    static constexpr int called_as() {
        int res = 0;
        if constexpr (std::is_lvalue_reference_v<This>)
            res |= CalledLvalue;
        if constexpr (std::is_const_v<std::remove_reference_t<This>>)
            res |= CalledConst;

        if constexpr (std::is_lvalue_reference_v<Value>)
            res |= PassedLvalue;
        if constexpr (std::is_const_v<std::remove_reference_t<Value>>)
            res |= PassedConst;

        return res;
    }

    template <typename T>
    constexpr int operator()(T&&) & {
        return called_as<F&, T>();
    }

    template <typename T>
    constexpr int operator()(T&&) const& {
        return called_as<F const&, T>();
    }

    template <typename T>
    constexpr int operator()(T&&) && {
        return called_as<F, T>();
    }

    template <typename T>
    constexpr int operator()(T&&) const&& {
        return called_as<F const, T>();
    }
};

constexpr bool test_forwarding() {
    __RXX optional<long> o = 1;
    F f;

    assert(*o.transform(f) == (PassedLvalue | CalledLvalue));
    assert(*o.transform(std::move(f)) == PassedLvalue);
    assert(*std::move(o).transform(f) == CalledLvalue);
    assert(*std::move(o).transform(std::move(f)) == 0);

    auto const& co = o;

    assert(*co.transform(f) == (PassedLvalue | PassedConst | CalledLvalue));
    assert(*co.transform(std::move(f)) == (PassedLvalue | PassedConst));
    assert(*std::move(co).transform(f) == (PassedConst | CalledLvalue));
    assert(*std::move(co).transform(std::move(f)) == PassedConst);

    auto const& cf = f;

    assert(*o.transform(cf) == (PassedLvalue | CalledLvalue | CalledConst));
    assert(*o.transform(std::move(cf)) == (PassedLvalue | CalledConst));
    assert(*std::move(o).transform(cf) == (CalledLvalue | CalledConst));
    assert(*std::move(o).transform(std::move(cf)) == CalledConst);

    assert(*co.transform(cf) ==
        (PassedLvalue | PassedConst | CalledLvalue | CalledConst));
    assert(*co.transform(std::move(cf)) ==
        (PassedLvalue | PassedConst | CalledConst));
    assert(*std::move(co).transform(cf) ==
        (PassedConst | CalledLvalue | CalledConst));
    assert(
        *std::move(co).transform(std::move(cf)) == (PassedConst | CalledConst));

    o = __RXX nullopt;

    assert(!o.transform(f).has_value());
    assert(!co.transform(f).has_value());
    assert(!std::move(o).transform(f).has_value());
    assert(!std::move(co).transform(f).has_value());

    return true;
}

static_assert(test_forwarding());

constexpr bool test_copy_elision() {
    struct immovable {
        constexpr immovable(int p) : power_level(p) {}
        immovable(immovable&&) = delete;

        int power_level;
    };

    struct Force {
        constexpr immovable operator()(int i) const { return {i + 1}; }
    };

    __RXX optional<int> irresistible(9000);
    __RXX optional<immovable> object = irresistible.transform(Force{});
    assert(object->power_level > 9000);

    return true;
}

static_assert(test_copy_elision());

void f(int&) {}

void test_unconstrained() {
    // PR libstdc++/102863 - Optional monadic ops should not be constrained
    __RXX optional<int> x;
    auto answer = x.transform([](auto& y) {
        f(y);
        return 42;
    });
    assert(!answer);
}

int main() {
    test_transform();
    test_forwarding();
    test_copy_elision();
    test_unconstrained();
}
