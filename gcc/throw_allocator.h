// Copyright 2025-2026 Bryan Wong
// Adapted from GCC testsuite

// Copyright (C) 2005-2026 Free Software Foundation, Inc.
//
// This file is part of the GNU ISO C++ Library.  This library is free
// software; you can redistribute it and/or modify it under the terms
// of the GNU General Public License as published by the Free Software
// Foundation; either version 3, or (at your option) any later
// version.

// This library is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.

// Under Section 7 of GPL version 3, you are granted additional
// permissions described in the GCC Runtime Library Exception, version
// 3.1, as published by the Free Software Foundation.

// You should have received a copy of the GNU General Public License and
// a copy of the GCC Runtime Library Exception along with this program;
// see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
// <http://www.gnu.org/licenses/>.

// Copyright (C) 2004 Ami Tavory and Vladimir Dreizin, IBM-HRL.

// Permission to use, copy, modify, sell, and distribute this software
// is hereby granted without fee, provided that the above copyright
// notice appears in all copies, and that both that copyright notice
// and this permission notice appear in supporting documentation. None
// of the above authors, nor IBM Haifa Research Laboratories, make any
// representation about the suitability of this software for any
// purpose. It is provided "as is" without express or implied
// warranty.

/** @file ext/throw_allocator.h
 *  This file is a GNU extension to the Standard C++ Library.
 *
 *  Contains two exception-generating types (throw_value, throw_allocator)
 *  intended to be used as value and allocator types while testing
 *  exception safety in templatized containers and algorithms. The
 *  allocator has additional log and debug features. The exception
 *  generated is of type forced_exception_error.
 */

#ifndef _THROW_ALLOCATOR_H
#  define _THROW_ALLOCATOR_H 1
#  include "rxx/config.h"

#  include <cmath>
#  include <ctime>
#  include <functional>
#  include <map>
#  include <ostream>
#  include <random>
#  include <stdexcept>
#  include <string>
#  include <type_traits>
#  include <utility>

#  if !__has_builtin(__builtin_sprintf)
#    include <cstdio>
#  endif

template <typename Alloc, typename = typename Alloc::value_type>
struct __alloc_traits : std::allocator_traits<Alloc> {
    typedef Alloc allocator_type;
    typedef std::allocator_traits<Alloc> base_type;
    typedef typename base_type::value_type value_type;
    typedef typename base_type::pointer pointer;
    typedef typename base_type::const_pointer const_pointer;
    typedef typename base_type::size_type size_type;
    typedef typename base_type::difference_type difference_type;
    // C++11 allocators do not define reference or const_reference
    typedef value_type& reference;
    typedef value_type const& const_reference;
    using base_type::allocate;
    using base_type::construct;
    using base_type::deallocate;
    using base_type::destroy;
    using base_type::max_size;

private:
    template <typename Ptr>
    using is_custom_pointer = std::conjunction<std::is_same<pointer, Ptr>,
        std::negation<std::is_pointer<Ptr>>>;

public:
    // overload construct for non-standard pointer types
    template <typename Ptr, typename... Args>
    RXX_ATTRIBUTE(ALWAYS_INLINE)
    static constexpr std::enable_if_t<is_custom_pointer<Ptr>::value> construct(
        Alloc& __a, Ptr __p,
        Args&&... __args) noexcept(noexcept(base_type::construct(__a,
        std::__to_address(__p), std::forward<Args>(__args)...))) {
        base_type::construct(
            __a, std::__to_address(__p), std::forward<Args>(__args)...);
    }

    // overload destroy for non-standard pointer types
    template <typename Ptr>
    RXX_ATTRIBUTE(ALWAYS_INLINE)
    static constexpr std::enable_if_t<is_custom_pointer<Ptr>::value> destroy(
        Alloc& __a, Ptr __p) noexcept(noexcept(base_type::destroy(__a,
        std::to_address(__p)))) {
        base_type::destroy(__a, std::__to_address(__p));
    }

    RXX_ATTRIBUTE(ALWAYS_INLINE)
    static constexpr Alloc _S_select_on_copy(Alloc const& __a) {
        return base_type::select_on_container_copy_construction(__a);
    }

    RXX_ATTRIBUTE(ALWAYS_INLINE)
    static constexpr void _S_on_swap(Alloc& a, Alloc& b) {
        if constexpr (base_type::propagate_on_container_swap::value) {
            using std::swap;
            swap(a, b);
        }
    }

    RXX_ATTRIBUTE(ALWAYS_INLINE)
    static constexpr bool _S_propagate_on_copy_assign() {
        return base_type::propagate_on_container_copy_assignment::value;
    }

    RXX_ATTRIBUTE(ALWAYS_INLINE)
    static constexpr bool _S_propagate_on_move_assign() {
        return base_type::propagate_on_container_move_assignment::value;
    }

    RXX_ATTRIBUTE(ALWAYS_INLINE)
    static constexpr bool _S_propagate_on_swap() {
        return base_type::propagate_on_container_swap::value;
    }

    RXX_ATTRIBUTE(ALWAYS_INLINE)
    static constexpr bool _S_always_equal() {
        return base_type::is_always_equal::value;
    }

    RXX_ATTRIBUTE(ALWAYS_INLINE) static constexpr bool _S_nothrow_move() {
        return _S_propagate_on_move_assign() || _S_always_equal();
    }

    template <typename T>
    struct rebind {
        typedef typename base_type::template rebind_alloc<T> other;
    };
};

/**
 *  @brief Thrown by utilities for testing exception safety.
 *  @ingroup exceptions
 */
struct forced_error : public std::exception {};

// Substitute for forced_error object when -fno-exceptions.
inline void __throw_forced_error() {
    RXX_THROW(forced_error());
}

/**
 *  @brief Base class for checking address and label information
 *  about allocations. Create a std::map between the allocated
 *  address (void*) and a datum for annotations, which are a pair of
 *  numbers corresponding to label and allocated size.
 */
struct annotate_base {
private:
    typedef std::pair<size_t, size_t> data_type;
    typedef std::map<void*, data_type> map_alloc_type;
    typedef map_alloc_type::value_type entry_type;
    typedef map_alloc_type::const_iterator const_iterator;
    typedef map_alloc_type::const_reference const_reference;
    typedef std::map<void*, size_t> map_construct_type;

public:
    annotate_base() {
        label();
        map_alloc();
    }

    static void set_label(size_t l) { label() = l; }

    static size_t get_label() { return label(); }

    void insert(void* p, size_t size) {
        entry_type entry = make_entry(p, size);
        if (!p) {
            std::string error("annotate_base::insert null insert!\n");
            log_to_string(error, entry);
            std::__throw_logic_error(error.c_str());
        }

        std::pair<map_alloc_type::iterator, bool> inserted =
            map_alloc().insert(entry);
        if (!inserted.second) {
            std::string error("annotate_base::insert double insert!\n");
            log_to_string(error, entry);
            log_to_string(error, *inserted.first);
            std::__throw_logic_error(error.c_str());
        }
    }

    void erase(void* p, size_t size) {
        map_alloc().erase(check_allocated(p, size));
    }

    void insert_construct(void* p) {
        if (!p) {
            std::string error("annotate_base::insert_construct null!\n");
            std::__throw_logic_error(error.c_str());
        }

        auto inserted = map_construct().insert(std::make_pair(p, get_label()));
        if (!inserted.second) {
            std::string error(
                "annotate_base::insert_construct double insert!\n");
            log_to_string(error, std::make_pair(p, get_label()));
            log_to_string(error, *inserted.first);
            std::__throw_logic_error(error.c_str());
        }
    }

    void erase_construct(void* p) {
        map_construct().erase(check_constructed(p));
    }

    // See if a particular address and allocation size has been saved.
    inline map_alloc_type::iterator check_allocated(void* p, size_t size) {
        map_alloc_type::iterator found = map_alloc().find(p);
        if (found == map_alloc().end()) {
            std::string error("annotate_base::check_allocated by value "
                              "null erase!\n");
            log_to_string(error, make_entry(p, size));
            std::__throw_logic_error(error.c_str());
        }

        if (found->second.second != size) {
            std::string error("annotate_base::check_allocated by value "
                              "wrong-size erase!\n");
            log_to_string(error, make_entry(p, size));
            log_to_string(error, *found);
            std::__throw_logic_error(error.c_str());
        }

        return found;
    }

    // See if a given label has been allocated.
    inline void check(size_t label) {
        std::string found;
        {
            const_iterator beg = map_alloc().begin();
            const_iterator end = map_alloc().end();
            while (beg != end) {
                if (beg->second.first == label)
                    log_to_string(found, *beg);
                ++beg;
            }
        }

        {
            auto beg = map_construct().begin();
            auto end = map_construct().end();
            while (beg != end) {
                if (beg->second == label)
                    log_to_string(found, *beg);
                ++beg;
            }
        }

        if (!found.empty()) {
            std::string error("annotate_base::check by label\n");
            error += found;
            std::__throw_logic_error(error.c_str());
        }
    }

    // See if there is anything left allocated or constructed.
    inline static void check() {
        std::string found;
        {
            const_iterator beg = map_alloc().begin();
            const_iterator end = map_alloc().end();
            while (beg != end) {
                log_to_string(found, *beg);
                ++beg;
            }
        }

        {
            auto beg = map_construct().begin();
            auto end = map_construct().end();
            while (beg != end) {
                log_to_string(found, *beg);
                ++beg;
            }
        }

        if (!found.empty()) {
            std::string error("annotate_base::check \n");
            error += found;
            std::__throw_logic_error(error.c_str());
        }
    }

    inline map_construct_type::iterator check_constructed(void* p) {
        auto found = map_construct().find(p);
        if (found == map_construct().end()) {
            std::string error("annotate_base::check_constructed not "
                              "constructed!\n");
            log_to_string(error, std::make_pair(p, get_label()));
            std::__throw_logic_error(error.c_str());
        }

        return found;
    }

    inline void check_constructed(size_t label) {
        auto beg = map_construct().begin();
        auto end = map_construct().end();
        std::string found;
        while (beg != end) {
            if (beg->second == label)
                log_to_string(found, *beg);
            ++beg;
        }

        if (!found.empty()) {
            std::string error("annotate_base::check_constructed by label\n");
            error += found;
            std::__throw_logic_error(error.c_str());
        }
    }

private:
    friend std::ostream& operator<<(std::ostream&, annotate_base const&);

    entry_type make_entry(void* p, size_t size) {
        return std::make_pair(p, data_type(get_label(), size));
    }

    static void log_to_string(std::string& s, const_reference ref) {
#  if !__has_builtin(__builtin_sprintf)
        decltype(&std::sprintf) __builtin_sprintf = &std::sprintf;
#  endif

        char buf[40];
        char const tab('\t');
        s += "label: ";
        unsigned long l = static_cast<unsigned long>(ref.second.first);
        __builtin_sprintf(buf, "%lu", l);
        s += buf;
        s += tab;
        s += "size: ";
        l = static_cast<unsigned long>(ref.second.second);
        __builtin_sprintf(buf, "%lu", l);
        s += buf;
        s += tab;
        s += "address: ";
        __builtin_sprintf(buf, "%p", ref.first);
        s += buf;
        s += '\n';
    }

    static void log_to_string(
        std::string& s, std::pair<void const*, size_t> const& ref) {
#  if !__has_builtin(__builtin_sprintf)
        auto __builtin_sprintf = &std::sprintf;
#  endif

        char buf[40];
        char const tab('\t');
        s += "label: ";
        unsigned long l = static_cast<unsigned long>(ref.second);
        __builtin_sprintf(buf, "%lu", l);
        s += buf;
        s += tab;
        s += "address: ";
        __builtin_sprintf(buf, "%p", ref.first);
        s += buf;
        s += '\n';
    }

    static size_t& label() {
        static size_t _S_label(std::numeric_limits<size_t>::max());
        return _S_label;
    }

    static map_alloc_type& map_alloc() {
        static map_alloc_type _S_map;
        return _S_map;
    }

    static map_construct_type& map_construct() {
        static map_construct_type _S_map;
        return _S_map;
    }
};

inline std::ostream& operator<<(std::ostream& os, annotate_base const& __b) {
    std::string error;
    typedef annotate_base base_type;
    {
        base_type::const_iterator beg = __b.map_alloc().begin();
        base_type::const_iterator end = __b.map_alloc().end();
        for (; beg != end; ++beg)
            __b.log_to_string(error, *beg);
    }
    {
        auto beg = __b.map_construct().begin();
        auto end = __b.map_construct().end();
        for (; beg != end; ++beg)
            __b.log_to_string(error, *beg);
    }

    return os << error;
}

/**
 *  @brief Base struct for condition policy.
 *
 * Requires a public member function with the signature
 * void throw_conditionally()
 */
struct condition_base {
    condition_base() = default;
    condition_base(condition_base const&) = default;
    condition_base& operator=(condition_base const&) = default;
    virtual ~condition_base() {};
};

/**
 *  @brief Base class for incremental control and throw.
 */
struct limit_condition : public condition_base {
    // Scope-level adjustor objects: set limit for throw at the
    // beginning of a scope block, and restores to previous limit when
    // object is destroyed on exiting the block.
    struct adjustor_base {
    private:
        size_t const _M_orig;

    public:
        adjustor_base() : _M_orig(limit()) {}

        virtual ~adjustor_base() { set_limit(_M_orig); }
    };

    /// Never enter the condition.
    struct never_adjustor : public adjustor_base {
        never_adjustor() { set_limit(std::numeric_limits<size_t>::max()); }
    };

    /// Always enter the condition.
    struct always_adjustor : public adjustor_base {
        always_adjustor() { set_limit(count()); }
    };

    /// Enter the nth condition.
    struct limit_adjustor : public adjustor_base {
        limit_adjustor(size_t const __l) { set_limit(__l); }
    };

    // Increment _S_count every time called.
    // If _S_count matches the limit count, throw.
    static void throw_conditionally() {
        if (count() == limit())
            __throw_forced_error();
        ++count();
    }

    static size_t& count() {
        static size_t _S_count(0);
        return _S_count;
    }

    static size_t& limit() {
        static size_t _S_limit(std::numeric_limits<size_t>::max());
        return _S_limit;
    }

    // Zero the throw counter, set limit to argument.
    static void set_limit(size_t const __l) {
        limit() = __l;
        count() = 0;
    }
};

/**
 *  @brief Base class for random probability control and throw.
 */
struct random_condition : public condition_base {
    // Scope-level adjustor objects: set probability for throw at the
    // beginning of a scope block, and restores to previous
    // probability when object is destroyed on exiting the block.
    struct adjustor_base {
    private:
        double const _M_orig;

    public:
        adjustor_base() : _M_orig(probability()) {}

        virtual ~adjustor_base() { set_probability(_M_orig); }
    };

    /// Group condition.
    struct group_adjustor : public adjustor_base {
        group_adjustor(size_t size) {
            set_probability(1 -
                std::pow(double(1 - probability()), double(0.5 / (size + 1))));
        }
    };

    /// Never enter the condition.
    struct never_adjustor : public adjustor_base {
        never_adjustor() { set_probability(0); }
    };

    /// Always enter the condition.
    struct always_adjustor : public adjustor_base {
        always_adjustor() { set_probability(1); }
    };

    random_condition() {
        probability();
        engine();
    }

    static void set_probability(double __p) { probability() = __p; }

    static void throw_conditionally() {
        if (generate() < probability())
            __throw_forced_error();
    }

    void seed(unsigned long __s) { engine().seed(__s); }

private:
    typedef std::uniform_real_distribution<double> distribution_type;
    typedef std::mt19937 engine_type;

    static double generate() {
        distribution_type const distribution(0, 1);
        static auto generator = std::bind(distribution, engine());

#  if !__has_builtin(__builtin_sprintf)
        decltype(&std::sprintf) __builtin_sprintf = &std::sprintf;
#  endif

        double random = generator();
        if (random < distribution.min() || random > distribution.max()) {
            std::string __s("random_condition::generate");
            __s += "\n";
            __s += "random number generated is: ";
            char buf[40];
            __builtin_sprintf(buf, "%f", random);
            __s += buf;
            std::__throw_out_of_range(__s.c_str());
        }

        return random;
    }

    static double& probability() {
        static double _S_p;
        return _S_p;
    }

    static engine_type& engine() {
        static engine_type _S_e;
        return _S_e;
    }
};

/**
 *  @brief Class with exception generation control. Intended to be
 *  used as a value_type in templatized code.
 *
 *  Note: Destructor not allowed to throw.
 */
template <typename Cond>
struct throw_value_base : public Cond {
    typedef Cond condition_type;

    using condition_type::throw_conditionally;

    std::size_t _M_i;

    throw_value_base() : _M_i(0) { throw_conditionally(); }

    throw_value_base(throw_value_base const& __v) : _M_i(__v._M_i) {
        throw_conditionally();
    }

    // Shall not throw.
    throw_value_base(throw_value_base&&) = default;

    explicit throw_value_base(std::size_t const __i) : _M_i(__i) {
        throw_conditionally();
    }

    throw_value_base& operator=(throw_value_base const& __v) {
        throw_conditionally();
        _M_i = __v._M_i;
        return *this;
    }

    // Shall not throw.
    throw_value_base& operator=(throw_value_base&&) = default;

    throw_value_base& operator++() {
        throw_conditionally();
        ++_M_i;
        return *this;
    }
};

template <typename Cond>
inline void swap(throw_value_base<Cond>& __a, throw_value_base<Cond>& __b) {
    typedef throw_value_base<Cond> throw_value;
    throw_value::throw_conditionally();
    throw_value orig(__a);
    __a = __b;
    __b = orig;
}

// General instantiable types requirements.
template <typename Cond>
inline bool operator==(
    throw_value_base<Cond> const& __a, throw_value_base<Cond> const& __b) {
    typedef throw_value_base<Cond> throw_value;
    throw_value::throw_conditionally();
    bool __ret = __a._M_i == __b._M_i;
    return __ret;
}

template <typename Cond>
inline bool operator<(
    throw_value_base<Cond> const& __a, throw_value_base<Cond> const& __b) {
    typedef throw_value_base<Cond> throw_value;
    throw_value::throw_conditionally();
    bool __ret = __a._M_i < __b._M_i;
    return __ret;
}

// Numeric algorithms instantiable types requirements.
template <typename Cond>
inline throw_value_base<Cond> operator+(
    throw_value_base<Cond> const& __a, throw_value_base<Cond> const& __b) {
    typedef throw_value_base<Cond> throw_value;
    throw_value::throw_conditionally();
    throw_value __ret(__a._M_i + __b._M_i);
    return __ret;
}

template <typename Cond>
inline throw_value_base<Cond> operator-(
    throw_value_base<Cond> const& __a, throw_value_base<Cond> const& __b) {
    typedef throw_value_base<Cond> throw_value;
    throw_value::throw_conditionally();
    throw_value __ret(__a._M_i - __b._M_i);
    return __ret;
}

template <typename Cond>
inline throw_value_base<Cond> operator*(
    throw_value_base<Cond> const& __a, throw_value_base<Cond> const& __b) {
    typedef throw_value_base<Cond> throw_value;
    throw_value::throw_conditionally();
    throw_value __ret(__a._M_i * __b._M_i);
    return __ret;
}

/// Type throwing via limit condition.
struct throw_value_limit : public throw_value_base<limit_condition> {
    typedef throw_value_base<limit_condition> base_type;

    throw_value_limit() {}

    throw_value_limit(throw_value_limit const& __other)
        : base_type(__other._M_i) {}

    throw_value_limit(throw_value_limit&&) = default;

    explicit throw_value_limit(std::size_t const __i) : base_type(__i) {}

    throw_value_limit& operator=(throw_value_limit const& __other) {
        base_type::operator=(__other);
        return *this;
    }

    throw_value_limit& operator=(throw_value_limit&&) = default;
};

/// Type throwing via random condition.
struct throw_value_random : public throw_value_base<random_condition> {
    typedef throw_value_base<random_condition> base_type;

    throw_value_random() {}

    throw_value_random(throw_value_random const& __other)
        : base_type(__other._M_i) {}

    throw_value_random(throw_value_random&&) = default;

    explicit throw_value_random(std::size_t const __i) : base_type(__i) {}

    throw_value_random& operator=(throw_value_random const& __other) {
        base_type::operator=(__other);
        return *this;
    }

    throw_value_random& operator=(throw_value_random&&) = default;
};

/**
 *  @brief Allocator class with logging and exception generation control.
 * Intended to be used as an allocator_type in templatized code.
 *  @ingroup allocators
 *
 *  Note: Deallocate not allowed to throw.
 */
template <typename T, typename Cond>
class throw_allocator_base : public annotate_base, public Cond {
public:
    typedef std::size_t size_type;
    typedef std::ptrdiff_t difference_type;
    typedef T value_type;
    typedef value_type* pointer;
    typedef value_type const* const_pointer;
    typedef value_type& reference;
    typedef value_type const& const_reference;

    typedef std::true_type propagate_on_container_move_assignment;

private:
    typedef Cond condition_type;

    std::allocator<value_type> _M_allocator;

    typedef __alloc_traits<std::allocator<value_type>> traits;

    using condition_type::throw_conditionally;

public:
    size_type max_size() const noexcept {
        return traits::max_size(_M_allocator);
    }

    pointer address(reference __x) const noexcept {
        return RXX_BUILTIN_addressof(__x);
    }

    const_pointer address(const_reference __x) const noexcept {
        return RXX_BUILTIN_addressof(__x);
    }

    [[nodiscard]] pointer allocate(size_type __n, void const* __hint = 0) {
        if (__n > this->max_size())
            std::__throw_bad_alloc();

        throw_conditionally();
        pointer const a = traits::allocate(_M_allocator, __n, __hint);
        insert(a, sizeof(value_type) * __n);
        return a;
    }

    template <typename _Up, typename... Args>
    void construct(_Up* __p, Args&&... __args) {
        traits::construct(_M_allocator, __p, std::forward<Args>(__args)...);
        insert_construct(__p);
    }

    template <typename _Up>
    void destroy(_Up* __p) {
        erase_construct(__p);
        traits::destroy(_M_allocator, __p);
    }

    void deallocate(pointer __p, size_type __n) {
        erase(__p, sizeof(value_type) * __n);
        _M_allocator.deallocate(__p, __n);
    }

    void check_allocated(pointer __p, size_type __n) {
        size_type __t = sizeof(value_type) * __n;
        annotate_base::check_allocated(__p, __t);
    }

    void check(size_type __n) { annotate_base::check(__n); }
};

template <typename T, typename Cond>
inline bool operator==(throw_allocator_base<T, Cond> const&,
    throw_allocator_base<T, Cond> const&) {
    return true;
}

/// Allocator throwing via limit condition.
template <typename T>
struct throw_allocator_limit : public throw_allocator_base<T, limit_condition> {
    template <typename T1>
    struct rebind {
        typedef throw_allocator_limit<T1> other;
    };

    throw_allocator_limit() noexcept = default;

    throw_allocator_limit(throw_allocator_limit const&) noexcept {}

    template <typename T1>
    throw_allocator_limit(throw_allocator_limit<T1> const&) noexcept {}

    ~throw_allocator_limit() noexcept = default;

    throw_allocator_limit& operator=(throw_allocator_limit const&) = default;
};

/// Allocator throwing via random condition.
template <typename T>
struct throw_allocator_random :
    public throw_allocator_base<T, random_condition> {
    template <typename T1>
    struct rebind {
        typedef throw_allocator_random<T1> other;
    };

    throw_allocator_random() noexcept = default;

    throw_allocator_random(throw_allocator_random const&) noexcept {}

    template <typename T1>
    throw_allocator_random(throw_allocator_random<T1> const&) noexcept {}

    ~throw_allocator_random() noexcept = default;

    throw_allocator_random& operator=(throw_allocator_random const&) = default;
};

RXX_STD_NAMESPACE_BEGIN

/// Explicit specialization of std::hash for throw_value_limit.
template <>
struct hash<throw_value_limit> {
    size_t operator()(throw_value_limit const& __val) const {
        throw_value_limit::throw_conditionally();
        std::hash<std::size_t> __h;
        size_t __result = __h(__val._M_i);
        return __result;
    }
};

/// Explicit specialization of std::hash for throw_value_random.
template <>
struct hash<throw_value_random> {
    size_t operator()(throw_value_random const& __val) const {
        throw_value_random::throw_conditionally();
        std::hash<std::size_t> __h;
        size_t __result = __h(__val._M_i);
        return __result;
    }
};

RXX_STD_NAMESPACE_END

#endif
