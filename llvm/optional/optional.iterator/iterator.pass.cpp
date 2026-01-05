// Copyright 2025 Bryan Wong
// Adapted from LLVM testsuite

//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// REQUIRES: std-at-least-c++26

// <optional>

// template <class T> class optional::iterator;
// template <class T> class optional::const_iterator;

#include "rxx/optional.h"
#include "rxx/ranges.h"

#include <cassert>
#include <type_traits>
#include <utility>

template <typename T>
concept has_iterator = requires { typename T::iterator; };

template <typename T>
concept has_const_iterator = requires { typename T::const_iterator; };

template <typename T>
concept has_both_iterators = has_iterator<T> && has_const_iterator<T>;

template <typename T>
concept only_has_iterator = has_iterator<T> && !has_const_iterator<T>;

template <typename T>
concept has_no_iterators = !has_iterator<T> && !has_const_iterator<T>;

template <typename T>
constexpr void test(std::decay_t<T> v) {
    __RXX optional<T> opt{v};
    { static_assert(__RXX ranges::range<decltype(opt)>); }

    { // Dereferencing an iterator of an engaged optional will return the same
      // value that the optional holds.
        auto it = opt.begin();
        auto it2 = std::as_const(opt).begin();
        assert(*it == *opt);
        assert(*it2 == *std::as_const(opt));
    }

    { // optional::iterator and optional::const_iterator satisfy the
      // Cpp17RandomAccessIterator and contiguous iterator.
        auto it = opt.begin();
        auto it2 = std::as_const(opt).begin();
        static_assert(std::contiguous_iterator<decltype(it)>);
        static_assert(std::contiguous_iterator<decltype(it2)>);

        static_assert(std::random_access_iterator<decltype(it)>);
        static_assert(std::random_access_iterator<decltype(it2)>);
    }

    { // const_iterator::value_type == std::remove_cvref_t<T>,
      // const_iterator::reference == const T&, iterator::value_type =
      // std::remove_cvref_t<T>, iterator::reference == T&
        // std::remove_cv_t is impossible for optional<T&>
        auto it = opt.begin();
        auto it2 = std::as_const(opt).begin();
        static_assert(std::is_same_v<typename decltype(it)::value_type,
            std::remove_cvref_t<T>>);
        static_assert(std::is_same_v<typename decltype(it)::reference,
            std::remove_reference_t<T>&>);
        static_assert(std::is_same_v<typename decltype(it2)::value_type,
            std::remove_cvref_t<T>>);

        // optional<T&> doesn't have const_iterator
        if constexpr (!std::is_lvalue_reference_v<T>) {
            static_assert(std::is_same_v<typename decltype(it2)::reference,
                std::remove_reference_t<T> const&>);
        }
    }

    { // __RXX ranges::size for an engaged optional<T> == 1, disengaged
        // optional<T> == 0
        __RXX optional<T> const disengaged{__RXX nullopt};
        __RXX optional<T> disengaged2{__RXX nullopt};
        assert(__RXX ranges::size(opt) == 1);
        assert(__RXX ranges::size(std::as_const(opt)) == 1);

        assert(__RXX ranges::size(disengaged) == 0);
        assert(__RXX ranges::size(disengaged2) == 0);
    }

#if RXX_SUPPORTS_RANGE_FORMAT
    { // __RXX ranges::enable_view<optional<T>> == true, and
      // std::format_kind<optional<T>> == std::range_format::disabled
        static_assert(__RXX ranges::enable_view<__RXX optional<T>> == true);
        static_assert(std::format_kind<__RXX optional<T>> ==
            std::range_format::disabled);
    }
#endif

    // An optional with value that is reset will have a begin() == end(), then
    // when it is reassigned a value, begin() != end(), and *begin() will
    // contain the new value.
    {
        __RXX optional<T> val{v};
        assert(val.begin() != val.end());
        val.reset();
        assert(val.begin() == val.end());
        val.emplace(v);
        assert(val.begin() != val.end());
        assert(*(val.begin()) == v);
    }
}

constexpr bool test() {
    // Verify that iterator and const_iterator are present for object type T,
    // but for T&, that only iterator is available iff T is an object type and
    // is not an unbounded array.

    static_assert(has_both_iterators<__RXX optional<int>>);
    static_assert(has_both_iterators<__RXX optional<int const>>);
#if RXX_SUPPORTS_OPTIONAL_REFERENCES
    static_assert(only_has_iterator<__RXX optional<int&>>);
    static_assert(only_has_iterator<__RXX optional<int const&>>);
    static_assert(only_has_iterator<__RXX optional<int (&)[1]>>);
#endif
    static_assert(has_no_iterators<__RXX optional<int (&)[]>>);
    static_assert(has_no_iterators<__RXX optional<int (&)()>>);

    test<int>(1);
    test<char>('a');
    test<bool>(true);
    test<int const>(2);
    test<char const>('b');
    test<int&>(1);
    test<char&>('a');
    test<bool&>(true);
    test<int const&>(2);
    test<char const&>('b');

    static_assert(!__RXX ranges::range<__RXX optional<int (&)()>>);
    static_assert(!__RXX ranges::range<__RXX optional<int (&)[]>>);
    static_assert(__RXX ranges::range<__RXX optional<int (&)[42]>>);

    return true;
}

int main(int, char**) {
    assert(test());
    static_assert(test());

    return 0;
}
