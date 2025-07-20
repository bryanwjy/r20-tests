// Copyright 2025 Bryan Wong
// Adapted from LLVM testsuite

//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// UNSUPPORTED: c++03, c++11, c++14, c++17

// <memory>
//
// namespace ranges {
//   template<class T, class... Args>
//     constexpr T* construct_at(T* location, Args&&... args); // since C++20
// }

#include "../static_asserts.h"
#include "../test_iterators.h"
#include "rxx/memory.h"

#include <cassert>
#include <initializer_list>
#include <type_traits>

static_assert(std::is_class_v<decltype(xranges::construct_at)>);

struct Foo {
    int x = 0;
    int y = 0;

    constexpr Foo() = default;
    constexpr explicit Foo(int set_x, int set_y) : x(set_x), y(set_y) {}
    constexpr Foo(std::initializer_list<int>);

    void operator&() const = delete;
    void operator,(auto&&) const = delete;
};

ASSERT_SAME_TYPE(decltype(xranges::construct_at((int*)nullptr)), int*);
ASSERT_SAME_TYPE(decltype(xranges::construct_at((Foo*)nullptr)), Foo*);

struct Counted {
    int& count;

    constexpr Counted(int& count_ref) : count(count_ref) { ++count; }
    constexpr Counted(Counted const& rhs) : count(rhs.count) { ++count; }
    constexpr ~Counted() { --count; }
};

constexpr bool test() {
    // Value initialization.
    {
        int x = 1;

        int* result = xranges::construct_at(&x);
        assert(result == &x);
        assert(x == 0);
    }

    // Copy initialization.
    {
        int x = 1;

        int* result = xranges::construct_at(&x, 42);
        assert(result == &x);
        assert(x == 42);
    }

    // Explicit multiargument constructor; also checks that the initializer list
    // constructor is not invoked.
    {
        Foo f;

        Foo* result = xranges::construct_at(std::addressof(f), 42, 123);
        assert(result == std::addressof(f));
        assert(f.x == 42);
        assert(f.y == 123);
    }

    // Works with buffers of uninitialized memory.
    {
        std::allocator<Counted> alloc;
        Counted* out = alloc.allocate(2);
        int count = 0;

        Counted* result = xranges::construct_at(out, count);
        assert(result == out);
        assert(count == 1);

        result = xranges::construct_at(out + 1, count);
        assert(result == out + 1);
        assert(count == 2);

        std::destroy(out, out + 1);
        alloc.deallocate(out, 2);
    }

    return true;
}

constexpr bool can_construct_at(auto&&... args)
requires requires { xranges::construct_at(decltype(args)(args)...); }
{
    return true;
}

constexpr bool can_construct_at(auto&&...) {
    return false;
}

// Check that SFINAE works.
static_assert(can_construct_at((Foo*)nullptr, 1, 2));
static_assert(!can_construct_at((Foo*)nullptr, 1));
static_assert(!can_construct_at((Foo*)nullptr, 1, 2, 3));
static_assert(!can_construct_at(nullptr, 1, 2));
static_assert(!can_construct_at((int*)nullptr, 1, 2));
static_assert(!can_construct_at(contiguous_iterator<Foo*>(), 1, 2));
// Can't construct function pointers.
static_assert(!can_construct_at((int (*)()) nullptr));
static_assert(!can_construct_at((int (*)()) nullptr, nullptr));
// TODO(varconst): check that array types work once D114649 implementing LWG3639
// lands.

int main(int, char**) {
    test();
    static_assert(test());

    return 0;
}
