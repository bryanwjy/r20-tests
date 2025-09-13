// Copyright 2025 Bryan Wong
// Adapted from GCC testsuite

// Copyright (C) 2023-2025 Free Software Foundation, Inc.
//
// This file is part of the GNU ISO C++ Library.  This library is free
// software; you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the
// Free Software Foundation; either version 3, or (at your option)
// any later version.

// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along
// with this library; see the file COPYING3.  If not see
// <http://www.gnu.org/licenses/>.

#include "rxx/generator.h"

#include <cassert>

namespace xranges = rxx::ranges;
namespace xviews = rxx::views;
template <typename... Ts>
using xgenerator = rxx::generator<Ts...>;

struct operation_watcher {
    int copy_ctor = 0;
    int move_ctor = 0;
    int copy_assign = 0;
    int move_assign = 0;
};

enum qualifier {
    none,
    lvalue_ref,
    rvalue_ref,
};

struct foo {
    operation_watcher* watcher;
    void (*check)(qualifier, foo const&);

    foo(operation_watcher& watcher, void (*check)(qualifier, foo const&))
        : watcher{&watcher}
        , check{check} {}

    foo(foo const& o) : watcher(o.watcher), check(o.check) {
        ++watcher->copy_ctor;
    }

    foo& operator=(foo const& o) {
        watcher = o.watcher;
        check = o.check;
        ++watcher->copy_assign;
        return *this;
    }

    foo(foo&& o) : watcher(o.watcher), check(o.check) {
        ++watcher->move_ctor;
        o.watcher = nullptr;
        o.check = nullptr;
    }

    foo& operator=(foo&& o) {
        watcher = o.watcher;
        check = o.check;
        ++watcher->copy_assign;
        o.watcher = nullptr;
        o.check = nullptr;
        return *this;
    }
};

xgenerator<foo> foogen() {
    operation_watcher watcher{};
    co_yield foo{watcher, [](qualifier q, foo const& instance) {
                     assert(instance.watcher->copy_ctor == 0);
                     assert(instance.watcher->copy_assign == 0);
                     assert(instance.watcher->move_assign == 0);
                     if (q == none) {
                         assert(instance.watcher->move_ctor == 1);
                     } else if (q == lvalue_ref || q == rvalue_ref) {
                         assert(instance.watcher->move_ctor == 0);
                     }
                 }};

    {
        foo f{watcher, [](qualifier q, foo const& instance) {
                  assert(instance.watcher->copy_ctor == 1);
                  assert(instance.watcher->copy_assign == 0);
                  assert(instance.watcher->move_assign == 0);
                  if (q == none) {
                      assert(instance.watcher->move_ctor == 2);
                  } else if (q == lvalue_ref || q == rvalue_ref) {
                      assert(instance.watcher->move_ctor == 0);
                  }
              }};
        co_yield f;
    }

    {
        foo const f{watcher, [](qualifier q, foo const& instance) {
                        assert(instance.watcher->copy_ctor == 2);
                        assert(instance.watcher->copy_assign == 0);
                        assert(instance.watcher->move_assign == 0);
                        if (q == none) {
                            assert(instance.watcher->move_ctor == 3);
                        } else if (q == lvalue_ref || q == rvalue_ref) {
                            assert(instance.watcher->move_ctor == 0);
                        }
                    }};
        co_yield f;
    }

    {
        foo f{watcher, [](qualifier q, foo const& instance) {
                  assert(instance.watcher->copy_ctor == 2);
                  assert(instance.watcher->copy_assign == 0);
                  assert(instance.watcher->move_assign == 0);
                  if (q == none) {
                      assert(instance.watcher->move_ctor == 4);
                  } else if (q == lvalue_ref || q == rvalue_ref) {
                      assert(instance.watcher->move_ctor == 0);
                  }
              }};
        co_yield std::move(f);
    }
}

xgenerator<foo const&> foogen2() {

    operation_watcher watcher{};
    co_yield foo{watcher, [](qualifier q, foo const& instance) {
                     assert(instance.watcher->copy_assign == 0);
                     assert(instance.watcher->move_ctor == 0);
                     assert(instance.watcher->move_assign == 0);
                     if (q == none) {
                         assert(instance.watcher->copy_ctor == 1);
                     } else if (q == lvalue_ref || q == rvalue_ref) {
                         assert(instance.watcher->copy_ctor == 0);
                     }
                 }};

    {
        foo f{watcher, [](qualifier q, foo const& instance) {
                  assert(instance.watcher->copy_assign == 0);
                  assert(instance.watcher->move_ctor == 0);
                  assert(instance.watcher->move_assign == 0);
                  if (q == none) {
                      assert(instance.watcher->copy_ctor == 2);
                  } else if (q == lvalue_ref || q == rvalue_ref) {
                      assert(instance.watcher->copy_ctor == 0);
                  }
              }};
        co_yield f;
    }

    {
        foo const f{watcher, [](qualifier q, foo const& instance) {
                        assert(instance.watcher->copy_assign == 0);
                        assert(instance.watcher->move_ctor == 0);
                        assert(instance.watcher->move_assign == 0);
                        if (q == none) {
                            assert(instance.watcher->copy_ctor == 3);
                        } else if (q == lvalue_ref || q == rvalue_ref) {
                            assert(instance.watcher->copy_ctor == 0);
                        }
                    }};
        co_yield f;
    }

    {
        foo f{watcher, [](qualifier q, foo const& instance) {
                  assert(instance.watcher->copy_assign == 0);
                  assert(instance.watcher->move_ctor == 0);
                  assert(instance.watcher->move_assign == 0);
                  if (q == none) {
                      assert(instance.watcher->copy_ctor == 4);
                  } else if (q == lvalue_ref || q == rvalue_ref) {
                      assert(instance.watcher->copy_ctor == 0);
                  }
              }};
        co_yield std::move(f);
    }
}

xgenerator<foo&&> foogen3() {
    operation_watcher watcher{};
    co_yield foo{watcher, [](qualifier q, foo const& instance) {
                     assert(instance.watcher->copy_assign == 0);
                     assert(instance.watcher->move_assign == 0);
                     if (q == none) {
                         assert(instance.watcher->copy_ctor == 0);
                         assert(instance.watcher->move_ctor == 1);
                     } else if (q == lvalue_ref || q == rvalue_ref) {
                         assert(instance.watcher->copy_ctor == 0);
                         assert(instance.watcher->move_ctor == 0);
                     }
                 }};

    {
        foo f{watcher, [](qualifier q, foo const& instance) {
                  assert(instance.watcher->copy_assign == 0);
                  assert(instance.watcher->move_assign == 0);
                  if (q == none) {
                      assert(instance.watcher->copy_ctor == 1);
                      assert(instance.watcher->move_ctor == 2);
                  } else if (q == lvalue_ref || q == rvalue_ref) {
                      assert(instance.watcher->copy_ctor == 1);
                      assert(instance.watcher->move_ctor == 0);
                  }
              }};
        co_yield f;
    }

    {
        foo const f{watcher, [](qualifier q, foo const& instance) {
                        assert(instance.watcher->copy_assign == 0);
                        assert(instance.watcher->move_assign == 0);
                        if (q == none) {
                            assert(instance.watcher->copy_ctor == 2);
                            assert(instance.watcher->move_ctor == 3);
                        } else if (q == lvalue_ref || q == rvalue_ref) {
                            assert(instance.watcher->copy_ctor == 2);
                            assert(instance.watcher->move_ctor == 0);
                        }
                    }};
        co_yield f;
    }

    {
        foo f{watcher, [](qualifier q, foo const& instance) {
                  assert(instance.watcher->copy_assign == 0);
                  assert(instance.watcher->move_assign == 0);
                  if (q == none) {
                      assert(instance.watcher->copy_ctor == 2);
                      assert(instance.watcher->move_ctor == 4);
                  } else if (q == lvalue_ref || q == rvalue_ref) {
                      assert(instance.watcher->copy_ctor == 2);
                      assert(instance.watcher->move_ctor == 0);
                  }
              }};
        co_yield std::move(f);
    }
}

int main() {

    for (auto f : foogen()) {
        assert(f.watcher != nullptr);
        f.check(qualifier::none, f);
    }

    for (auto const& f : foogen()) {
        assert(f.watcher != nullptr);
        f.check(qualifier::lvalue_ref, f);
    }

    for (auto&& f : foogen()) {
        assert(f.watcher != nullptr);
        f.check(qualifier::rvalue_ref, f);
    }

    for (auto f : foogen2()) {
        assert(f.watcher != nullptr);
        f.check(qualifier::none, f);
    }

    for (auto const& f : foogen2()) {
        assert(f.watcher != nullptr);
        f.check(qualifier::lvalue_ref, f);
    }

    for (auto&& f : foogen2()) {
        assert(f.watcher != nullptr);
        f.check(qualifier::rvalue_ref, f);
    }

    for (auto f : foogen3()) {
        assert(f.watcher != nullptr);
        f.check(qualifier::none, f);
    }

    for (auto const& f : foogen3()) {
        assert(f.watcher != nullptr);
        f.check(qualifier::lvalue_ref, f);
    }

    for (auto&& f : foogen3()) {
        assert(f.watcher != nullptr);
        f.check(qualifier::rvalue_ref, f);
    }
}
