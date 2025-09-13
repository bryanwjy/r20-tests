// Copyright 2025 Bryan Wong
// Adapted from GCC testsuite

// Copyright (C) 2024-2025 Free Software Foundation, Inc.
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
template <typename... Ts>
using xgenerator = rxx::generator<Ts...>;

// Check that the return type of iterator::operator* is the reference type.
// Pre-op* return type fix, this'd have resulted in a op* return type of const
// bool&.

xgenerator<bool, bool> foo();

static_assert(std::is_same_v<decltype(*foo().begin()), bool>);
static_assert(std::is_same_v<typename decltype(foo())::yielded, bool const&>);
