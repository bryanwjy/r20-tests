// Copyright 2025-2026 Bryan Wong
// Adapted from LLVM testsuite
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// UNSUPPORTED: c++03, c++11, c++14
// <optional>

// optional<T>& operator=(nullopt_t) noexcept;

#include "../../../archetypes.h"
#include "rxx/optional.h"

#include <cassert>
#include <type_traits>

using __RXX nullopt;
using __RXX optional;

constexpr bool test() {
    enum class State {
        inactive,
        constructed,
        destroyed
    };
    State state = State::inactive;

    struct StateTracker {
        constexpr StateTracker(State& s) : state_(&s) {
            s = State::constructed;
        }
        constexpr ~StateTracker() { *state_ = State::destroyed; }

        State* state_;
    };
    {
        optional<int> opt;
        static_assert(noexcept(opt = nullopt) == true, "");
        opt = nullopt;
        assert(static_cast<bool>(opt) == false);
    }
    {
        optional<int> opt(3);
        opt = nullopt;
        assert(static_cast<bool>(opt) == false);
    }
    {
        optional<StateTracker> opt;
        opt = nullopt;
        assert(state == State::inactive);
        assert(static_cast<bool>(opt) == false);
    }
    {
        optional<StateTracker> opt(state);
        assert(state == State::constructed);
        opt = nullopt;
        assert(state == State::destroyed);
        assert(static_cast<bool>(opt) == false);
    }
    return true;
}

int main(int, char**) {
    static_assert(test());
    test();
    using TT = TestTypes::TestType;
    TT::reset();
    {
        optional<TT> opt;
        static_assert(noexcept(opt = nullopt) == true, "");
        assert(TT::destroyed == 0);
        opt = nullopt;
        assert(TT::constructed == 0);
        assert(TT::alive == 0);
        assert(TT::destroyed == 0);
        assert(static_cast<bool>(opt) == false);
    }
    assert(TT::alive == 0);
    assert(TT::destroyed == 0);
    TT::reset();
    {
        optional<TT> opt(42);
        assert(TT::destroyed == 0);
        TT::reset_constructors();
        opt = nullopt;
        assert(TT::constructed == 0);
        assert(TT::alive == 0);
        assert(TT::destroyed == 1);
        assert(static_cast<bool>(opt) == false);
    }
    assert(TT::alive == 0);
    assert(TT::destroyed == 1);
    TT::reset();

    return 0;
}
