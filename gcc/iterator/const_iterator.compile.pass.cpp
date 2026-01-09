// Copyright 2025 Bryan Wong
// Adapted from GCC testsuite

// Copyright (C) 2020-2025 Free Software Foundation, Inc.
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

// PR libstdc++/112490 - infinite meta error in
// reverse_iterator<basic_const_iterator<vector<int>::iterator>>

#include "rxx/iterator.h"

#include <vector>

using I = std::vector<int>::iterator;
using CI = __RXX basic_const_iterator<I>;
using RCI = std::reverse_iterator<CI>;
static_assert(std::totally_ordered<RCI>);
