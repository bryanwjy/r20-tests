// Copyright 2025-2026 Bryan Wong
// Adapted from LLVM testsuite
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// UNSUPPORTED: c++03, c++11, c++14, c++17

// <optional>

// The following special member functions should propagate the triviality of
// the element held in the optional (see P0602R4):
//
// constexpr optional(const optional& rhs);
// constexpr optional(optional&& rhs) noexcept(see below);
// constexpr optional<T>& operator=(const optional& rhs);
// constexpr optional<T>& operator=(optional&& rhs) noexcept(see below);

#include "../../archetypes.h"
#include "rxx/optional.h"

#include <type_traits>

constexpr bool implies(bool p, bool q) {
    return !p || q;
}

template <class T>
struct SpecialMemberTest {
    using O = __RXX optional<T>;

    static_assert(implies(std::is_trivially_copy_constructible_v<T>,
                      std::is_trivially_copy_constructible_v<O>),
        "optional<T> is trivially copy constructible if T is trivially copy "
        "constructible.");

    static_assert(implies(std::is_trivially_move_constructible_v<T>,
                      std::is_trivially_move_constructible_v<O>),
        "optional<T> is trivially move constructible if T is trivially move "
        "constructible");

    static_assert(implies(std::is_trivially_copy_constructible_v<T> &&
                          std::is_trivially_copy_assignable_v<T> &&
                          std::is_trivially_destructible_v<T>,

                      std::is_trivially_copy_assignable_v<O>),
        "optional<T> is trivially copy assignable if T is "
        "trivially copy constructible, "
        "trivially copy assignable, and "
        "trivially destructible");

    static_assert(implies(std::is_trivially_move_constructible_v<T> &&
                          std::is_trivially_move_assignable_v<T> &&
                          std::is_trivially_destructible_v<T>,

                      std::is_trivially_move_assignable_v<O>),
        "optional<T> is trivially move assignable if T is "
        "trivially move constructible, "
        "trivially move assignable, and"
        "trivially destructible.");
};

template <class... Args>
static void sink(Args&&...) {}

template <class... TestTypes>
struct DoTestsMetafunction {
    DoTestsMetafunction() { sink(SpecialMemberTest<TestTypes>{}...); }
};

struct TrivialMoveNonTrivialCopy {
    TrivialMoveNonTrivialCopy() = default;
    TrivialMoveNonTrivialCopy(TrivialMoveNonTrivialCopy const&) {}
    TrivialMoveNonTrivialCopy(TrivialMoveNonTrivialCopy&&) = default;
    TrivialMoveNonTrivialCopy& operator=(TrivialMoveNonTrivialCopy const&) {
        return *this;
    }
    TrivialMoveNonTrivialCopy& operator=(TrivialMoveNonTrivialCopy&&) = default;
};

struct TrivialCopyNonTrivialMove {
    TrivialCopyNonTrivialMove() = default;
    TrivialCopyNonTrivialMove(TrivialCopyNonTrivialMove const&) = default;
    TrivialCopyNonTrivialMove(TrivialCopyNonTrivialMove&&) {}
    TrivialCopyNonTrivialMove& operator=(
        TrivialCopyNonTrivialMove const&) = default;
    TrivialCopyNonTrivialMove& operator=(TrivialCopyNonTrivialMove&&) {
        return *this;
    }
};

int main(int, char**) {
    sink(ImplicitTypes::ApplyTypes<DoTestsMetafunction>{},
        ExplicitTypes::ApplyTypes<DoTestsMetafunction>{},
        NonLiteralTypes::ApplyTypes<DoTestsMetafunction>{},
        NonTrivialTypes::ApplyTypes<DoTestsMetafunction>{},
        DoTestsMetafunction<TrivialMoveNonTrivialCopy,
            TrivialCopyNonTrivialMove>{});
    return 0;
}
