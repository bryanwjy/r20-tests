// Copyright 2025-2026 Bryan Wong
// Adapted from GCC testsuite

#include "rxx/optional.h"

#include <cassert>

constexpr bool test_assign() {
    __RXX optional<int> oi(1);
    __RXX optional<unsigned> ou(2u), ou3(3u);

    // optional& operator=(nullopt_t);
    oi = __RXX nullopt;
    assert(!oi.has_value());
    oi = __RXX nullopt;
    assert(!oi.has_value());

    struct S {
        constexpr S() {}
        constexpr S(char, int, unsigned) {}
    };
    __RXX optional<S> os1, os2;

    // template<class U = T> optional& operator=(U&&);
    os1 = {'0', 1, 2u};
    assert(os1.has_value());
    os2 = {'3', 4, 5u};
    assert(os2.has_value());
    oi = 0u;
    assert(*oi == 0);
    oi = 1u;
    assert(*oi == 1);

    // template<class U> optional& operator=(const optional<U>&);
    oi = ou;
    assert(*oi == 2);
    oi = ou3;
    assert(*oi == 3);

    // template<class U> optional& operator=(optional<U>&&);
    oi = std::move(ou);
    assert(*oi == 2);
    oi = std::move(ou);
    assert(*oi == 2);
    oi = std::move(ou3);
    assert(*oi == 3);

    return true;
}

static_assert(test_assign());

constexpr bool test_emplace() {
    struct S {
        constexpr S(int i) : val(i) {}
        constexpr S(int i, int j) : val(i + j) {}
        constexpr S(std::initializer_list<char> l, int i = 0) : val(i) {
            for (char c : l)
                val -= c;
        }

        int val;

        constexpr bool operator==(int i) const { return val == i; }
    };

    __RXX optional<S> os;

    // template<class... Args> constexpr T& emplace(Args&&...);
    os.emplace(1);
    assert(*os == 1);
    os.emplace(2);
    assert(*os == 2);
    os.emplace(2, 3);
    assert(*os == 5);

    // template<class U, class... Args>
    // constexpr T& emplace(initializer_list<U>, Args&&...);
    os.emplace({'3', '4', '5'});
    assert(*os == -156);
    os.emplace({'6', '7', '8'}, 25);
    assert(*os == -140);

    return true;
}

static_assert(test_emplace());
