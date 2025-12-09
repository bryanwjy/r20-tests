// Copyright 2025 Bryan Wong
// Adapted from LLVM testsuite

//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef TEST_SUPPORT_ARCHETYPES_H
#define TEST_SUPPORT_ARCHETYPES_H

#include "test_workarounds.h"

#include <cassert>
#include <initializer_list>
#include <type_traits>

namespace ArchetypeBases {

template <bool, class T>
struct DepType : T {};

struct NullBase {
#ifndef TEST_WORKAROUND_MSVC_BROKEN_ZA_CTOR_CHECK
protected:
#endif // !TEST_WORKAROUND_MSVC_BROKEN_ZA_CTOR_CHECK
    NullBase() = default;
    NullBase(NullBase const&) = default;
    NullBase& operator=(NullBase const&) = default;
    NullBase(NullBase&&) = default;
    NullBase& operator=(NullBase&&) = default;
};

template <class Derived, bool Explicit = false>
struct TestBase {
    static inline int alive = 0;
    static inline int constructed = 0;
    static inline int value_constructed = 0;
    static inline int default_constructed = 0;
    static inline int copy_constructed = 0;
    static inline int move_constructed = 0;
    static inline int assigned = 0;
    static inline int value_assigned = 0;
    static inline int copy_assigned = 0;
    static inline int move_assigned = 0;
    static inline int destroyed = 0;

    static void reset() {
        assert(alive == 0);
        alive = 0;
        reset_constructors();
    }

    static void reset_constructors() {
        constructed = value_constructed = default_constructed =
            copy_constructed = move_constructed = 0;
        assigned = value_assigned = copy_assigned = move_assigned = destroyed =
            0;
    }

    TestBase() noexcept : value(0) {
        ++alive;
        ++constructed;
        ++default_constructed;
    }
    template <bool Dummy = true,
        typename std::enable_if<Dummy && Explicit, bool>::type = true>
    explicit TestBase(int x) noexcept : value(x) {
        ++alive;
        ++constructed;
        ++value_constructed;
    }
    template <bool Dummy = true,
        typename std::enable_if<Dummy && !Explicit, bool>::type = true>
    TestBase(int x) noexcept : value(x) {
        ++alive;
        ++constructed;
        ++value_constructed;
    }
    template <bool Dummy = true,
        typename std::enable_if<Dummy && Explicit, bool>::type = true>
    explicit TestBase(int, int y) noexcept : value(y) {
        ++alive;
        ++constructed;
        ++value_constructed;
    }
    template <bool Dummy = true,
        typename std::enable_if<Dummy && !Explicit, bool>::type = true>
    TestBase(int, int y) noexcept : value(y) {
        ++alive;
        ++constructed;
        ++value_constructed;
    }
    template <bool Dummy = true,
        typename std::enable_if<Dummy && Explicit, bool>::type = true>
    explicit TestBase(std::initializer_list<int>& il, int = 0) noexcept
        : value(static_cast<int>(il.size())) {
        ++alive;
        ++constructed;
        ++value_constructed;
    }
    template <bool Dummy = true,
        typename std::enable_if<Dummy && !Explicit, bool>::type = true>
    explicit TestBase(std::initializer_list<int>& il, int = 0) noexcept
        : value(static_cast<int>(il.size())) {
        ++alive;
        ++constructed;
        ++value_constructed;
    }
    TestBase& operator=(int xvalue) noexcept {
        value = xvalue;
        ++assigned;
        ++value_assigned;
        return *this;
    }
#ifndef TEST_WORKAROUND_MSVC_BROKEN_ZA_CTOR_CHECK
protected:
#endif // !TEST_WORKAROUND_MSVC_BROKEN_ZA_CTOR_CHECK
    ~TestBase() {
        assert(value != -999);
        assert(alive > 0);
        --alive;
        ++destroyed;
        value = -999;
    }
    explicit TestBase(TestBase const& o) noexcept : value(o.value) {
        assert(o.value != -1);
        assert(o.value != -999);
        ++alive;
        ++constructed;
        ++copy_constructed;
    }
    explicit TestBase(TestBase&& o) noexcept : value(o.value) {
        assert(o.value != -1);
        assert(o.value != -999);
        ++alive;
        ++constructed;
        ++move_constructed;
        o.value = -1;
    }
    TestBase& operator=(TestBase const& o) noexcept {
        assert(o.value != -1);
        assert(o.value != -999);
        ++assigned;
        ++copy_assigned;
        value = o.value;
        return *this;
    }
    TestBase& operator=(TestBase&& o) noexcept {
        assert(o.value != -1);
        assert(o.value != -999);
        ++assigned;
        ++move_assigned;
        value = o.value;
        o.value = -1;
        return *this;
    }

public:
    int value;
};

template <bool Explicit = false>
struct ValueBase {
    template <bool Dummy = true>
    requires (Dummy && Explicit)
    explicit constexpr ValueBase(int x) : value(x) {}

    template <bool Dummy = true>
    requires (Dummy && !Explicit)
    constexpr ValueBase(int x) : value(x) {}
    template <bool Dummy = true>
    requires (Dummy && Explicit)
    explicit constexpr ValueBase(int, int y) : value(y) {}

    template <bool Dummy = true>
    requires (Dummy && !Explicit)
    constexpr ValueBase(int, int y) : value(y) {}

    template <bool Dummy = true>
    requires (Dummy && Explicit)
    explicit constexpr ValueBase(std::initializer_list<int>& il, int = 0)
        : value(static_cast<int>(il.size())) {}

    template <bool Dummy = true>
    requires (Dummy && !Explicit)
    constexpr ValueBase(std::initializer_list<int>& il, int = 0)
        : value(static_cast<int>(il.size())) {}

    constexpr ValueBase& operator=(int xvalue) noexcept {
        value = xvalue;
        return *this;
    }
    //~ValueBase() { assert(value != -999); value = -999; }
    int value;
#ifndef TEST_WORKAROUND_MSVC_BROKEN_ZA_CTOR_CHECK
protected:
#endif // !TEST_WORKAROUND_MSVC_BROKEN_ZA_CTOR_CHECK
    constexpr static int check_value(int const& val) {
        assert(val != -1);
        assert(val != 999);
        return val;
    }
    constexpr static int check_value(int& val, int val_cp = 0) {
        assert(val != -1);
        assert(val != 999);
        val_cp = val;
        val = -1;
        return val_cp;
    }
    constexpr ValueBase() noexcept : value(0) {}
    constexpr ValueBase(ValueBase const& o) noexcept
        : value(check_value(o.value)) {}
    constexpr ValueBase(ValueBase&& o) noexcept : value(check_value(o.value)) {}
    constexpr ValueBase& operator=(ValueBase const& o) noexcept {
        assert(o.value != -1);
        assert(o.value != -999);
        value = o.value;
        return *this;
    }
    constexpr ValueBase& operator=(ValueBase&& o) noexcept {
        assert(o.value != -1);
        assert(o.value != -999);
        value = o.value;
        o.value = -1;
        return *this;
    }
};

template <bool Explicit = false>
struct TrivialValueBase {
    template <bool Dummy = true>
    requires (Dummy && Explicit)
    explicit constexpr TrivialValueBase(int x) : value(x) {}
    template <bool Dummy = true>
    requires (Dummy && !Explicit)
    constexpr TrivialValueBase(int x) : value(x) {}
    template <bool Dummy = true>
    requires (Dummy && Explicit)
    explicit constexpr TrivialValueBase(int, int y) : value(y) {}
    template <bool Dummy = true>
    requires (Dummy && !Explicit)
    constexpr TrivialValueBase(int, int y) : value(y) {}
    template <bool Dummy = true>
    requires (Dummy && Explicit)
    explicit constexpr TrivialValueBase(std::initializer_list<int>& il, int = 0)
        : value(static_cast<int>(il.size())) {}
    template <bool Dummy = true>
    requires (Dummy && !Explicit)
    constexpr TrivialValueBase(std::initializer_list<int>& il, int = 0)
        : value(static_cast<int>(il.size())) {}
    int value;
#ifndef TEST_WORKAROUND_MSVC_BROKEN_ZA_CTOR_CHECK
protected:
#endif // !TEST_WORKAROUND_MSVC_BROKEN_ZA_CTOR_CHECK
    constexpr TrivialValueBase() noexcept : value(0) {}
};

} // namespace ArchetypeBases

//============================================================================//
// Trivial Implicit Test Types
namespace ImplicitTypes {
#include "archetypes.ipp"
}

//============================================================================//
// Trivial Explicit Test Types
namespace ExplicitTypes {
#define DEFINE_EXPLICIT explicit
#include "archetypes.ipp"
} // namespace ExplicitTypes

//============================================================================//
//
namespace NonConstexprTypes {
#define DEFINE_CONSTEXPR
#include "archetypes.ipp"
} // namespace NonConstexprTypes

//============================================================================//
// Non-literal implicit test types
namespace NonLiteralTypes {
#define DEFINE_ASSIGN_CONSTEXPR
#define DEFINE_DTOR(Name) \
    ~Name() {}
#include "archetypes.ipp"
} // namespace NonLiteralTypes

//============================================================================//
// Non-throwing implicit test types
namespace NonThrowingTypes {
#define DEFINE_NOEXCEPT noexcept
#include "archetypes.ipp"
} // namespace NonThrowingTypes

//============================================================================//
// Non-Trivially Copyable Implicit Test Types
namespace NonTrivialTypes {
#define DEFINE_CTOR \
    {}
#define DEFINE_ASSIGN \
    { return *this; }
#include "archetypes.ipp"
} // namespace NonTrivialTypes

//============================================================================//
// Implicit counting types
namespace TestTypes {
#define DEFINE_CONSTEXPR
#define DEFINE_BASE(Name) ::ArchetypeBases::TestBase<Name>
#include "archetypes.ipp"

using TestType = AllCtors;

// Add equality operators
template <class Tp>
constexpr bool operator==(Tp const& L, Tp const& R) noexcept {
    return L.value == R.value;
}

template <class Tp>
constexpr bool operator!=(Tp const& L, Tp const& R) noexcept {
    return L.value != R.value;
}

} // namespace TestTypes

//============================================================================//
// Implicit counting types
namespace ExplicitTestTypes {
#define DEFINE_CONSTEXPR
#define DEFINE_EXPLICIT explicit
#define DEFINE_BASE(Name) ::ArchetypeBases::TestBase<Name, true>
#include "archetypes.ipp"

using TestType = AllCtors;

// Add equality operators
template <class Tp>
constexpr bool operator==(Tp const& L, Tp const& R) noexcept {
    return L.value == R.value;
}

template <class Tp>
constexpr bool operator!=(Tp const& L, Tp const& R) noexcept {
    return L.value != R.value;
}

} // namespace ExplicitTestTypes

//============================================================================//
// Implicit value types
namespace ConstexprTestTypes {
#define DEFINE_BASE(Name) ::ArchetypeBases::ValueBase<>
#include "archetypes.ipp"

using TestType = AllCtors;

// Add equality operators
template <class Tp>
constexpr bool operator==(Tp const& L, Tp const& R) noexcept {
    return L.value == R.value;
}

template <class Tp>
constexpr bool operator!=(Tp const& L, Tp const& R) noexcept {
    return L.value != R.value;
}

} // namespace ConstexprTestTypes

//============================================================================//
//
namespace ExplicitConstexprTestTypes {
#define DEFINE_EXPLICIT explicit
#define DEFINE_BASE(Name) ::ArchetypeBases::ValueBase<true>
#include "archetypes.ipp"

using TestType = AllCtors;

// Add equality operators
template <class Tp>
constexpr bool operator==(Tp const& L, Tp const& R) noexcept {
    return L.value == R.value;
}

template <class Tp>
constexpr bool operator!=(Tp const& L, Tp const& R) noexcept {
    return L.value != R.value;
}

} // namespace ExplicitConstexprTestTypes

//============================================================================//
//
namespace TrivialTestTypes {
#define DEFINE_BASE(Name) ::ArchetypeBases::TrivialValueBase<false>
#include "archetypes.ipp"

using TestType = AllCtors;

// Add equality operators
template <class Tp>
constexpr bool operator==(Tp const& L, Tp const& R) noexcept {
    return L.value == R.value;
}

template <class Tp>
constexpr bool operator!=(Tp const& L, Tp const& R) noexcept {
    return L.value != R.value;
}

} // namespace TrivialTestTypes

//============================================================================//
//
namespace ExplicitTrivialTestTypes {
#define DEFINE_EXPLICIT explicit
#define DEFINE_BASE(Name) ::ArchetypeBases::TrivialValueBase<true>
#include "archetypes.ipp"

using TestType = AllCtors;

// Add equality operators
template <class Tp>
constexpr bool operator==(Tp const& L, Tp const& R) noexcept {
    return L.value == R.value;
}

template <class Tp>
constexpr bool operator!=(Tp const& L, Tp const& R) noexcept {
    return L.value != R.value;
}

} // namespace ExplicitTrivialTestTypes

#endif // TEST_SUPPORT_ARCHETYPES_H
