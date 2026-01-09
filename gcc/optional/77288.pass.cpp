// Copyright 2025-2026 Bryan Wong
// Adapted from GCC testsuite

// Copyright (C) 2016-2026 Free Software Foundation, Inc.
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

#include "rxx/optional.h"

#include <any>
#include <cassert>

using __RXX optional;

void test01() {
    optional<optional<int>> nested_element;
    optional<int> element = {};
    nested_element = element;
    assert(nested_element);
}

template <class T>
struct service_result {
    static optional<T> get_result() {
        T sr;
        return sr;
    }

    static optional<T> get_result_with_cond(bool cond) {
        if (cond)
            return T{};
        return {};
    }
};

void test02() {
    assert(service_result<int>::get_result());
    assert(service_result<optional<int>>::get_result());
    assert(service_result<int>::get_result_with_cond(true));
    assert(service_result<optional<int>>::get_result_with_cond(true));
    assert(!service_result<int>::get_result_with_cond(false));
    assert(!service_result<optional<int>>::get_result_with_cond(false));
}

struct Widget {
    Widget(int) {}
    Widget(optional<int>) {}
};

void test03() {
    optional<Widget> w;
    w = optional<int>();
    assert(w);
    static_assert(!std::is_assignable_v<optional<Widget>&, optional<short>>);
    ;
    w = optional<optional<int>>();
    assert(!w);
    static_assert(
        !std::is_assignable_v<optional<Widget>&, optional<optional<short>>>);
    ;

    optional<Widget> w2{optional<int>()};
    assert(w2);
    optional<Widget> w3 = optional<int>();
    assert(w3);
    optional<Widget> w4{optional<short>()};
    assert(w4);
    static_assert(!std::is_convertible_v<optional<short>&&, optional<Widget>>);

    optional<Widget> w6{optional<optional<int>>()};
    assert(!w6);
    optional<Widget> w7 = optional<optional<int>>();
    assert(!w7);
    optional<Widget> w8{optional<optional<short>>()};
    assert(!w8);
    static_assert(
        !std::is_convertible_v<optional<optional<short>>&&, optional<Widget>>);
    optional<Widget> w10{optional<optional<short>>(10)};
    assert(w10);
    optional<Widget> w11 = __RXX nullopt;
    assert(!w11);
    optional<Widget> w12 = {};
    assert(!w12);
    optional<Widget> w13{__RXX nullopt};
    assert(!w13);
    optional<Widget> w14;
    w14 = {};
    assert(!w14);
}

struct Widget2 {
    Widget2(int) {}
    Widget2(optional<int>) {}
    Widget2& operator=(int) { return *this; }
    Widget2& operator=(optional<int>) { return *this; }
};

void test04() {
    optional<Widget2> w;
    w = optional<int>();
    assert(w);
    w = optional<short>();
    assert(w);
    w = optional<optional<int>>();
    assert(!w);
    w = optional<optional<short>>();
    assert(!w);
    w = optional<optional<short>>(10);
    optional<Widget2> w2 = __RXX nullopt;
    assert(!w2);
    optional<Widget2> w3 = {};
    assert(!w3);
    optional<Widget2> w4{__RXX nullopt};
    assert(!w4);
    optional<Widget2> w5;
    w5 = {};
    assert(!w5);
}

struct Thingy {
    Thingy(int) {}
    Thingy(Widget) {}
};

void test05() {
    optional<Thingy> ot;

    static_assert(!std::is_assignable_v<optional<Thingy>&, optional<int>>);
    static_assert(std::is_assignable_v<optional<Thingy>&, optional<short>>);
    static_assert(
        !std::is_assignable_v<optional<Thingy>&, optional<optional<int>>>);
    ot = optional<Widget>();
    assert(!ot);
    optional<Thingy> ot2{optional<int>()};
    assert(ot2);
    static_assert(!std::is_convertible_v<optional<int>&&, optional<Thingy>>);
    optional<Thingy> ot3{optional<short>()};
    assert(!ot3);
    optional<Thingy> ot4 = optional<short>();
    assert(!ot4);

    optional<Thingy> ot5{optional<optional<int>>()};
    assert(!ot5);
    static_assert(
        !std::is_convertible_v<optional<optional<int>>&&, optional<Thingy>>);

    optional<Thingy> ot7{optional<Widget>()};
    assert(!ot7);
    optional<Thingy> ot8 = optional<Widget>();
    assert(!ot8);
    static_assert(
        !std::is_constructible_v<optional<Thingy>, optional<optional<short>>>);
    static_assert(
        !std::is_convertible_v<optional<optional<short>>, optional<Thingy>>);
    static_assert(
        !std::is_assignable_v<optional<Thingy>&, optional<optional<short>>>);
    optional<Thingy> ot9 = __RXX nullopt;
    assert(!ot9);
    optional<Thingy> ot10 = {};
    assert(!ot10);
    optional<Thingy> ot11{__RXX nullopt};
    assert(!ot11);
    optional<Thingy> ot12;
    ot12 = {};
    assert(!ot12);
}

struct RvalueConstructible {
    RvalueConstructible(int) {}
    RvalueConstructible(optional<int>&&) {}
};

void test06() {
    optional<int> oi;
    optional<RvalueConstructible> ori;
    static_assert(
        !std::is_assignable_v<optional<RvalueConstructible>&, optional<int>&>);
    ori = std::move(oi);
    assert(ori);

    optional<optional<int>> ooi;
    static_assert(!std::is_assignable_v<optional<RvalueConstructible>&,
        optional<optional<int>>&>);
    ori = std::move(ooi);
    assert(!ori);

    static_assert(!std::is_constructible_v<optional<RvalueConstructible>,
        optional<int>&>);
    static_assert(
        !std::is_convertible_v<optional<int>&, optional<RvalueConstructible>>);

    optional<RvalueConstructible> ori2(std::move(oi));
    assert(ori2);
    optional<RvalueConstructible> ori3 = std::move(oi);
    assert(ori3);

    static_assert(!std::is_constructible_v<optional<RvalueConstructible>,
        optional<optional<int>>&>);
    static_assert(!std::is_convertible_v<optional<optional<int>>&,
        optional<RvalueConstructible>>);
    optional<RvalueConstructible> ori6(std::move(ooi));
    assert(!ori6);
    optional<RvalueConstructible> ori7 = std::move(ooi);
    assert(!ori7);
    optional<RvalueConstructible> ori8 = __RXX nullopt;
    assert(!ori8);
    optional<RvalueConstructible> ori9 = {};
    assert(!ori9);
    optional<RvalueConstructible> ori10{__RXX nullopt};
    assert(!ori10);
    optional<RvalueConstructible> ori11;
    ori11 = {};
    assert(!ori11);
}

struct Thingy2 {
    Thingy2(int) {}
    explicit Thingy2(optional<int>) {}
    Thingy2(Widget) {}
};

void test07() {
    optional<Thingy2> ot{optional<int>{}};
    assert(ot);
    static_assert(!std::is_convertible_v<optional<int>, optional<Thingy2>>);
    optional<Thingy2> ot2{optional<short>{}};
    assert(ot2);
    static_assert(!std::is_convertible_v<optional<short>, optional<Thingy2>>);
    optional<Thingy2> ot3{optional<optional<int>>{}};
    assert(!ot3);
    static_assert(
        !std::is_convertible_v<optional<optional<int>>, optional<Thingy2>>);
    optional<Thingy2> ot4{optional<optional<short>>{}};
    assert(!ot4);
    static_assert(
        !std::is_convertible_v<optional<optional<short>>, optional<Thingy2>>);

    optional<Thingy2> ot5{optional<Widget>{}};
    assert(!ot5);
    optional<Thingy2> ot6 = optional<Widget>();
    assert(!ot6);

    static_assert(!std::is_assignable_v<optional<Thingy2>&, optional<int>>);
    static_assert(!std::is_assignable_v<optional<Thingy2>&, optional<short>>);
    static_assert(
        !std::is_assignable_v<optional<Thingy2>&, optional<optional<int>>>);
    static_assert(
        !std::is_assignable_v<optional<Thingy2>&, optional<optional<short>>>);
    optional<Thingy2> ot7;
    ot = optional<Widget>();
    assert(!ot7);
    optional<Thingy2> ot8 = __RXX nullopt;
    assert(!ot8);
    optional<Thingy2> ot9 = {};
    assert(!ot9);
    optional<Thingy2> ot10{__RXX nullopt};
    assert(!ot10);
    optional<Thingy2> ot11;
    ot11 = {};
    assert(!ot11);
}

struct Thingy3 {
    Thingy3(int) {}
    template <class... Args,
        std::enable_if_t<std::is_constructible_v<Widget, Args&&...>, bool> =
            true>
    explicit Thingy3(Args&&... args) {}
    Thingy3(Widget) {}
};

void test08() {
    optional<Thingy3> ot{optional<int>{}};
    assert(ot);
    static_assert(!std::is_convertible_v<optional<int>, optional<Thingy3>>);
    optional<Thingy3> ot2{optional<short>{}};
    assert(ot2);
    static_assert(!std::is_convertible_v<optional<short>, optional<Thingy3>>);
    optional<Thingy3> ot3{optional<optional<int>>{}};
    assert(!ot3);
    static_assert(
        !std::is_convertible_v<optional<optional<int>>, optional<Thingy3>>);
    optional<Thingy3> ot4{optional<optional<short>>{}};
    assert(!ot4);
    static_assert(
        !std::is_convertible_v<optional<optional<short>>, optional<Thingy3>>);

    optional<Thingy3> ot5{optional<Widget>{}};
    assert(!ot5);
    optional<Thingy3> ot6 = optional<Widget>();
    assert(!ot6);

    static_assert(!std::is_assignable_v<optional<Thingy3>&, optional<int>>);
    static_assert(!std::is_assignable_v<optional<Thingy3>&, optional<short>>);
    static_assert(
        !std::is_assignable_v<optional<Thingy3>&, optional<optional<int>>>);
    static_assert(
        !std::is_assignable_v<optional<Thingy3>&, optional<optional<short>>>);
    optional<Thingy3> ot7;
    ot = optional<Widget>();
    assert(!ot7);
    optional<Thingy3> ot8 = __RXX nullopt;
    assert(!ot8);
    optional<Thingy3> ot9 = {};
    assert(!ot9);
    optional<Thingy3> ot10{__RXX nullopt};
    assert(!ot10);
    optional<Thingy3> ot11;
    ot11 = {};
    assert(!ot11);
}

void test09() {
    std::any a = 42;
    optional<std::any> oa2 = a;
    assert(oa2);
    assert(std::any_cast<int>(*oa2) == 42);
    optional<std::any> oa3 = oa2;
    assert(oa3);
    assert(std::any_cast<int>(*oa3) == 42);
    optional<std::any> oa4{oa2};
    assert(oa4);
    assert(std::any_cast<int>(*oa4) == 42);
    optional<std::any> oa5(oa2);
    assert(oa5);
    assert(std::any_cast<int>(*oa5) == 42);
    optional<std::any> oa6;
    assert(!oa6);
    optional<std::any> oa7 = oa6;
    assert(!oa7);
    optional<std::any> oa8{oa6};
    assert(!oa8);
    optional<std::any> oa9(oa6);
    assert(!oa9);
}

void test10() {
    struct X {};
    optional<int> oi(std::in_place);
    oi = {};
    assert(oi.has_value() == false);
    optional<X> ot(std::in_place);
    ot = {};
    assert(ot.has_value() == false);
    optional<int> oi2(std::in_place);
    short int si = 6;
    oi2 = si;
}

int main() {
    test01();
    test02();
    test03();
    test04();
    test05();
    test06();
    test07();
    test08();
    test09();
    test10();
}
