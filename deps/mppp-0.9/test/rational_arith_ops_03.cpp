// Copyright 2016-2018 Francesco Biscani (bluescarni@gmail.com)
//
// This file is part of the mp++ library.
//
// This Source Code Form is subject to the terms of the Mozilla
// Public License v. 2.0. If a copy of the MPL was not distributed
// with this file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <cmath>
#include <cstddef>
#include <gmp.h>
#include <limits>
#include <stdexcept>
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>

#include <mp++/config.hpp>
#include <mp++/detail/type_traits.hpp>
#include <mp++/integer.hpp>
#include <mp++/rational.hpp>

#include "test_utils.hpp"

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

using namespace mppp;
using namespace mppp_test;

using sizes = std::tuple<std::integral_constant<std::size_t, 1>, std::integral_constant<std::size_t, 2>,
                         std::integral_constant<std::size_t, 3>, std::integral_constant<std::size_t, 6>,
                         std::integral_constant<std::size_t, 10>>;

struct rel_tester {
    template <typename S>
    void operator()(const S &) const
    {
        using rational = rational<S::value>;
        using integer = typename rational::int_t;
        rational n1{4}, n2{-2};

        REQUIRE(n1 != n2);
        REQUIRE(n1 == n1);
        REQUIRE(rational{} == rational{});
        REQUIRE(rational{} == 0);
        REQUIRE(0 == rational{});
        REQUIRE(wchar_t{0} == rational{});
        REQUIRE(n1 == 4);
        REQUIRE(n1 == wchar_t{4});
        REQUIRE(n1 == integer{4});
        REQUIRE(integer{4} == n1);
        REQUIRE(4u == n1);
        REQUIRE(n1 != 3);
        REQUIRE(n1 != wchar_t{3});
        REQUIRE(wchar_t{3} != n1);
        REQUIRE(static_cast<signed char>(-3) != n1);
        REQUIRE(4ull == n1);
        REQUIRE(-2 == n2);
        REQUIRE(n2 == short(-2));
        REQUIRE(-2.f == n2);
        REQUIRE(n2 == -2.f);
        REQUIRE(-3.f != n2);
        REQUIRE(n2 != -3.f);
        REQUIRE(-2. == n2);
        REQUIRE(n2 == -2.);
        REQUIRE(-3. != n2);
        REQUIRE(n2 != -3.);
#if defined(MPPP_WITH_MPFR)
        REQUIRE(-2.l == n2);
        REQUIRE(n2 == -2.l);
        REQUIRE(-3.l != n2);
        REQUIRE(n2 != -3.l);
#endif
#if defined(MPPP_HAVE_GCC_INT128)
        REQUIRE(__int128_t{2} == rational{2});
        REQUIRE(rational{2} == __int128_t{2});
        REQUIRE(__uint128_t{2} == rational{2});
        REQUIRE(rational{2} == __uint128_t{2});
        REQUIRE(__int128_t{3} != rational{2});
        REQUIRE(rational{3} != __int128_t{2});
        REQUIRE(__uint128_t{3} != rational{2});
        REQUIRE(rational{3} != __uint128_t{2});
#endif

        REQUIRE(n2 < n1);
        REQUIRE(n2 < 0);
        REQUIRE(n2 < wchar_t{0});
        REQUIRE(wchar_t{0} < n1);
        REQUIRE(n2 < integer{0});
        REQUIRE(integer{-100} < n2);
        REQUIRE(-3 < n2);
        REQUIRE(n2 < 0u);
        REQUIRE(-3ll < n2);
        REQUIRE(n2 < 0.f);
        REQUIRE(-3.f < n2);
        REQUIRE(n2 < 0.);
        REQUIRE(-3. < n2);
#if defined(MPPP_WITH_MPFR)
        REQUIRE(n2 < 0.l);
        REQUIRE(-3.l < n2);
#endif
#if defined(MPPP_HAVE_GCC_INT128)
        REQUIRE(__int128_t{2} < rational{4});
        REQUIRE(rational{2} < __int128_t{3});
        REQUIRE(__uint128_t{2} < rational{4});
        REQUIRE(rational{2} < __uint128_t{3});
#endif

        REQUIRE(n1 > n2);
        REQUIRE(0 > n2);
        REQUIRE(wchar_t{0} > n2);
        REQUIRE(n1 > wchar_t{0});
        REQUIRE(integer{0} > n2);
        REQUIRE(n2 > integer{-150});
        REQUIRE(n2 > -3);
        REQUIRE(0u > n2);
        REQUIRE(n2 > -3ll);
        REQUIRE(0.f > n2);
        REQUIRE(n2 > -3.f);
        REQUIRE(0. > n2);
        REQUIRE(n2 > -3.);
#if defined(MPPP_WITH_MPFR)
        REQUIRE(0.l > n2);
        REQUIRE(n2 > -3.l);
#endif
#if defined(MPPP_HAVE_GCC_INT128)
        REQUIRE(__int128_t{6} > rational{4});
        REQUIRE(rational{7} > __int128_t{3});
        REQUIRE(__uint128_t{5} > rational{4});
        REQUIRE(rational{34} > __uint128_t{3});
#endif

        REQUIRE(n2 <= n1);
        REQUIRE(n1 <= n1);
        REQUIRE(rational{} <= rational{});
        REQUIRE(rational{} <= 0);
        REQUIRE(rational{} <= wchar_t{0});
        REQUIRE(wchar_t{0} <= rational{});
        REQUIRE(0 <= rational{});
        REQUIRE(rational{} <= integer{0});
        REQUIRE(integer{0} <= rational{});
        REQUIRE(-2 <= n2);
        REQUIRE(n2 <= -2);
        REQUIRE(n2 <= 0);
        REQUIRE(-3 <= n2);
        REQUIRE(n2 <= 0u);
        REQUIRE(-3ll <= n2);
        REQUIRE(n2 <= 0.f);
        REQUIRE(-3.f <= n2);
        REQUIRE(-2.f <= n2);
        REQUIRE(n2 <= -2.f);
        REQUIRE(n2 <= 0.);
        REQUIRE(-3. <= n2);
        REQUIRE(-2. <= n2);
        REQUIRE(n2 <= -2.);
#if defined(MPPP_WITH_MPFR)
        REQUIRE(n2 <= 0.l);
        REQUIRE(-3.l <= n2);
        REQUIRE(-2.l <= n2);
        REQUIRE(n2 <= -2.l);
#endif
#if defined(MPPP_HAVE_GCC_INT128)
        REQUIRE(__int128_t{2} <= rational{4});
        REQUIRE(rational{2} <= __int128_t{2});
        REQUIRE(__uint128_t{2} <= rational{4});
        REQUIRE(rational{2} <= __uint128_t{2});
#endif

        REQUIRE(n1 >= n2);
        REQUIRE(n1 >= n1);
        REQUIRE(rational{} >= rational{});
        REQUIRE(rational{} >= 0);
        REQUIRE(rational{} >= wchar_t{0});
        REQUIRE(wchar_t{0} >= rational{});
        REQUIRE(0 >= rational{});
        REQUIRE(rational{} >= integer{0});
        REQUIRE(integer{0} >= rational{});
        REQUIRE(-2 >= n2);
        REQUIRE(n2 >= -2);
        REQUIRE(0 >= n2);
        REQUIRE(n2 >= -3);
        REQUIRE(0u >= n2);
        REQUIRE(n2 >= -3ll);
        REQUIRE(0.f >= n2);
        REQUIRE(n2 >= -3.f);
        REQUIRE(-2.f >= n2);
        REQUIRE(n2 >= -2.f);
        REQUIRE(0. >= n2);
        REQUIRE(n2 >= -3.);
        REQUIRE(-2. >= n2);
        REQUIRE(n2 >= -2.);
#if defined(MPPP_WITH_MPFR)
        REQUIRE(0.l >= n2);
        REQUIRE(n2 >= -3.l);
        REQUIRE(-2.l >= n2);
        REQUIRE(n2 >= -2.l);
#endif
#if defined(MPPP_HAVE_GCC_INT128)
        REQUIRE(__int128_t{5} >= rational{4});
        REQUIRE(rational{2} >= __int128_t{2});
        REQUIRE(__uint128_t{8} >= rational{4});
        REQUIRE(rational{2} >= __uint128_t{2});
#endif
    }
};

TEST_CASE("rel")
{
    tuple_for_each(sizes{}, rel_tester{});
}

struct incdec_tester {
    template <typename S>
    void operator()(const S &) const
    {
        using rational = rational<S::value>;
        rational q;
        REQUIRE(++q == 1);
        REQUIRE(q++ == 1);
        REQUIRE(q == 2);
        REQUIRE(--q == 1);
        REQUIRE(q-- == 1);
        REQUIRE(q == 0);
        REQUIRE(--q == -1);
        q = rational{-23, 7};
        REQUIRE(++q == rational{-16, 7});
        REQUIRE(q++ == rational{-16, 7});
        REQUIRE(++q == rational{-2, 7});
        REQUIRE(++q == rational{5, 7});
        REQUIRE(--q == rational{-2, 7});
        REQUIRE(--q == rational{-9, 7});
        REQUIRE(--q == rational{-16, 7});
        REQUIRE(q-- == rational{-16, 7});
        REQUIRE(q == rational{-23, 7});
    }
};

TEST_CASE("incdec")
{
    tuple_for_each(sizes{}, incdec_tester{});
}
