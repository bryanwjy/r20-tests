// Copyright 2025 Bryan Wong
// Adapted from LLVM testsuite

//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// UNSUPPORTED: c++03, c++11, c++14, c++17, c++20, c++23

// <variant>

// class variant;
// template<class Self, class Visitor>
//   constexpr decltype(auto) visit(this Self&&, Visitor&&); // since C++26
// template<class R, class Self, class Visitor>
//   constexpr R visit(this Self&&, Visitor&&);              // since C++26

#include "rxx/variant.h"

struct Incomplete;
template <class T>
struct Holder {
#if !RXX_COMPILER_GCC | RXX_COMPILER_GCC_AT_LEAST(14, 2, 0)
    T t;
#endif
};

constexpr bool test(bool do_it) {
    if (do_it) {
        __RXX variant<Holder<Incomplete>*, int> v = nullptr;

        v.visit([](auto) {});
        v.visit([](auto) -> Holder<Incomplete>* { return nullptr; });
        v.visit<void>([](auto) {});
        v.visit<void*>([](auto) -> Holder<Incomplete>* { return nullptr; });
    }
    return true;
}

int main(int, char**) {
    test(true);
    static_assert(test(true));

    return 0;
}
