// Copyright 2016-2018 Francesco Biscani (bluescarni@gmail.com)
//
// This file is part of the mp++ library.
//
// This Source Code Form is subject to the terms of the Mozilla
// Public License v. 2.0. If a copy of the MPL was not distributed
// with this file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <mp++/config.hpp>

#include <atomic>
#include <cmath>
#include <cstddef>
#include <gmp.h>
#include <iostream>
#include <limits>
#include <mp++/detail/type_traits.hpp>
#include <mp++/integer.hpp>
#include <random>
#include <sstream>
#include <stdexcept>
#include <string>
#if MPPP_CPLUSPLUS >= 201703L
#include <string_view>
#endif
#include <thread>
#include <tuple>
#include <type_traits>
#include <utility>

#include "test_utils.hpp"

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

static int ntries = 1000;

using namespace mppp;
using namespace mppp_test;

using int_types = std::tuple<char, signed char, unsigned char, short, unsigned short, int, unsigned, long,
                             unsigned long, long long, unsigned long long, wchar_t
#if defined(MPPP_HAVE_GCC_INT128)
                             ,
                             __uint128_t, __int128_t
#endif
                             >;

using fp_types = std::tuple<float, double
#if defined(MPPP_WITH_MPFR)
                            ,
                            long double
#endif
                            >;

using sizes = std::tuple<std::integral_constant<std::size_t, 1>, std::integral_constant<std::size_t, 2>,
                         std::integral_constant<std::size_t, 3>, std::integral_constant<std::size_t, 6>,
                         std::integral_constant<std::size_t, 10>>;

// A seed that will be used to init rngs in the multithreaded tests. Each time a batch of N threads
// finishes, this value gets bumped up by N, so that the next time a multithreaded test which uses rng
// is launched it will be inited with a different seed.
static std::mt19937::result_type mt_rng_seed(0u);

static std::mt19937 rng;

struct no_const {
};

struct nbits_ctor_tester {
    template <typename S>
    void operator()(const S &) const
    {
        using integer = integer<S::value>;
        integer_nbits_init ini;
        REQUIRE((integer{ini, 0}.is_static()));
        REQUIRE((integer{ini, 0}.is_zero()));
        REQUIRE((integer{ini, 1}.is_static()));
        REQUIRE((integer{ini, 1}.is_zero()));
        REQUIRE((integer{ini, 2}.is_static()));
        REQUIRE((integer{ini, 2}.is_zero()));
        REQUIRE((integer{ini, GMP_NUMB_BITS}.is_static()));
        REQUIRE((integer{ini, GMP_NUMB_BITS}.is_zero()));
        if (S::value == 1) {
            REQUIRE((integer{ini, GMP_NUMB_BITS + 1}.is_dynamic()));
            REQUIRE((integer{ini, GMP_NUMB_BITS + 1}.is_zero()));
            REQUIRE((integer{ini, GMP_NUMB_BITS + 1}.get_mpz_t()->_mp_alloc == 2));
            REQUIRE((integer{ini, GMP_NUMB_BITS + 2}.is_dynamic()));
            REQUIRE((integer{ini, GMP_NUMB_BITS + 2}.is_zero()));
            REQUIRE((integer{ini, GMP_NUMB_BITS + 2}.get_mpz_t()->_mp_alloc == 2));
            REQUIRE((integer{ini, GMP_NUMB_BITS * 2}.is_dynamic()));
            REQUIRE((integer{ini, GMP_NUMB_BITS * 2}.is_zero()));
            REQUIRE((integer{ini, GMP_NUMB_BITS * 2}.get_mpz_t()->_mp_alloc == 2));
            REQUIRE((integer{ini, GMP_NUMB_BITS * 2 + 1}.is_dynamic()));
            REQUIRE((integer{ini, GMP_NUMB_BITS * 2 + 1}.is_zero()));
            REQUIRE((integer{ini, GMP_NUMB_BITS * 2 + 1}.get_mpz_t()->_mp_alloc == 3));
        }
        REQUIRE((integer{ini, GMP_NUMB_BITS * S::value}.is_static()));
        REQUIRE((integer{ini, GMP_NUMB_BITS * S::value}.is_zero()));
        REQUIRE((integer{ini, GMP_NUMB_BITS * S::value + 1}.is_dynamic()));
        REQUIRE((integer{ini, GMP_NUMB_BITS * S::value + 1}.is_zero()));
        REQUIRE((integer{ini, GMP_NUMB_BITS * S::value + 1}.get_mpz_t()->_mp_alloc == S::value + 1));
    }
};

TEST_CASE("nbits constructor")
{
    tuple_for_each(sizes{}, nbits_ctor_tester{});
}

struct copy_move_tester {
    template <typename S>
    void operator()(const S &) const
    {
        using integer = integer<S::value>;
        integer n;
        REQUIRE(n.is_static());
        n = 123;
        REQUIRE(n.is_static());
        integer m{n};
        REQUIRE(n.is_static());
        REQUIRE(m.is_static());
        REQUIRE(n == 123);
        REQUIRE(m == 123);
        m.promote();
        REQUIRE(m.is_dynamic());
        integer m2{std::move(m)};
        REQUIRE(m2.is_dynamic());
        REQUIRE(m.is_static());
        REQUIRE(m == 0);
        m = 123;
        integer m3{std::move(m)};
        REQUIRE(m3 == 123);
        REQUIRE(m.is_static());
        REQUIRE(m3.is_static());
        m3.promote();
        integer m4{m3};
        REQUIRE(m3 == 123);
        REQUIRE(m4 == 123);
        REQUIRE(m3.is_dynamic());
        REQUIRE(m4.is_dynamic());
        m4 = m4;
        REQUIRE(m4.is_dynamic());
        REQUIRE(m4 == 123);
        m4 = std::move(m4);
        REQUIRE(m4.is_dynamic());
        REQUIRE(m4 == 123);
        integer m5{12}, m6{-10};
        m5 = m6;
        REQUIRE(m5.is_static());
        REQUIRE(m5 == -10);
        m5 = m4;
        REQUIRE(m5.is_dynamic());
        REQUIRE(m5 == 123);
        m4 = m6;
        REQUIRE(m4.is_static());
        REQUIRE(m4 == -10);
        m4.promote();
        m5 = m4;
        REQUIRE(m5.is_dynamic());
        REQUIRE(m5 == -10);
        m4 = std::move(m5);
        REQUIRE(m4.is_dynamic());
        REQUIRE(m4 == -10);
        m4 = integer{-1};
        REQUIRE(m4.is_static());
        REQUIRE(m4 == -1);
        m4.promote();
        m5 = 10;
        m5.promote();
        m4 = std::move(m5);
        REQUIRE(m4.is_dynamic());
        REQUIRE(m4 == 10);
        m5 = -1;
        m5 = std::move(m4);
        REQUIRE(m4.is_static());
        REQUIRE(m4 == 0);
        REQUIRE(m5.is_dynamic());
        REQUIRE(m5 == 10);
    }
};

TEST_CASE("copy and move")
{
    tuple_for_each(sizes{}, copy_move_tester{});
}

struct mpz_copy_ass_tester {
    template <typename S>
    void operator()(const S &) const
    {
        using integer = integer<S::value>;
        integer n;
        mpz_raii m;
        n = &m.m_mpz;
        REQUIRE(lex_cast(n) == "0");
        ::mpz_set_si(&m.m_mpz, 1234);
        n = &m.m_mpz;
        REQUIRE(n == 1234);
        ::mpz_set_si(&m.m_mpz, -1234);
        n = &m.m_mpz;
        REQUIRE(n == -1234);
        ::mpz_set_str(&m.m_mpz, "3218372891372987328917389127389217398271983712987398127398172389712937819237", 10);
        n = &m.m_mpz;
        REQUIRE(n == integer("3218372891372987328917389127389217398271983712987398127398172389712937819237"));
        ::mpz_set_str(&m.m_mpz, "-3218372891372987328917389127389217398271983712987398127398172389712937819237", 10);
        n = &m.m_mpz;
        REQUIRE(n == integer("-3218372891372987328917389127389217398271983712987398127398172389712937819237"));
        // Random testing.
        std::atomic<bool> fail(false);
        auto f = [&fail](unsigned u) {
            std::uniform_int_distribution<long> dist(nl_min<long>(), nl_max<long>());
            std::uniform_int_distribution<int> sdist(0, 1);
            std::mt19937 eng(static_cast<std::mt19937::result_type>(u + mt_rng_seed));
            for (auto i = 0; i < ntries; ++i) {
                mpz_raii mpz;
                auto tmp = dist(eng);
                ::mpz_set_si(&mpz.m_mpz, tmp);
                integer z;
                if (sdist(eng)) {
                    z.promote();
                }
                z = &mpz.m_mpz;
                if (z != tmp) {
                    fail.store(false);
                }
            }
        };
        std::thread t0(f, 0u), t1(f, 1u), t2(f, 2u), t3(f, 3u);
        t0.join();
        t1.join();
        t2.join();
        t3.join();
        REQUIRE(!fail.load());
        mt_rng_seed += 4u;
    }
};

TEST_CASE("mpz_t copy assignment")
{
    tuple_for_each(sizes{}, mpz_copy_ass_tester{});
}

#if !defined(_MSC_VER)

struct mpz_move_ass_tester {
    template <typename S>
    void operator()(const S &) const
    {
        using integer = integer<S::value>;
        integer n;
        ::mpz_t m0;
        ::mpz_init(m0);
        n = std::move(m0);
        REQUIRE(lex_cast(n) == "0");
        ::mpz_init(m0);
        ::mpz_set_si(m0, 1234);
        n = std::move(m0);
        REQUIRE(n == 1234);
        ::mpz_init(m0);
        ::mpz_set_si(m0, -1234);
        n = std::move(m0);
        REQUIRE(n == -1234);
        ::mpz_init(m0);
        ::mpz_set_str(m0, "3218372891372987328917389127389217398271983712987398127398172389712937819237", 10);
        n = std::move(m0);
        REQUIRE(n == integer("3218372891372987328917389127389217398271983712987398127398172389712937819237"));
        ::mpz_init(m0);
        ::mpz_set_str(m0, "-3218372891372987328917389127389217398271983712987398127398172389712937819237", 10);
        n = std::move(m0);
        REQUIRE(n == integer("-3218372891372987328917389127389217398271983712987398127398172389712937819237"));
        // Random testing.
        std::atomic<bool> fail(false);
        auto f = [&fail](unsigned u) {
            std::uniform_int_distribution<long> dist(nl_min<long>(), nl_max<long>());
            std::uniform_int_distribution<int> sdist(0, 1);
            std::mt19937 eng(static_cast<std::mt19937::result_type>(u + mt_rng_seed));
            for (auto i = 0; i < ntries; ++i) {
                ::mpz_t m1;
                ::mpz_init(m1);
                auto tmp = dist(eng);
                ::mpz_set_si(m1, tmp);
                integer z;
                if (sdist(eng)) {
                    z.promote();
                }
                z = std::move(m1);
                if (z != tmp) {
                    fail.store(false);
                }
            }
        };
        std::thread t0(f, 0u), t1(f, 1u), t2(f, 2u), t3(f, 3u);
        t0.join();
        t1.join();
        t2.join();
        t3.join();
        REQUIRE(!fail.load());
        mt_rng_seed += 4u;
    }
};

TEST_CASE("mpz_t move assignment")
{
    tuple_for_each(sizes{}, mpz_move_ass_tester{});
}

#endif

struct string_ass_tester {
    template <typename S>
    void operator()(const S &) const
    {
        using integer = integer<S::value>;
        integer n;
        n = "123";
        REQUIRE(n == 123);
        n = " -456 ";
        REQUIRE(n == -456);
        n = std::string("123");
        REQUIRE(n == 123);
        n = std::string(" -456 ");
        REQUIRE(n == -456);
        REQUIRE_THROWS_PREDICATE(n = "", std::invalid_argument, [](const std::invalid_argument &ia) {
            return std::string(ia.what()) == "The string '' is not a valid integer in base 10";
        });
#if MPPP_CPLUSPLUS >= 201703L
        n = std::string_view(" -123 ");
        REQUIRE(n == -123);
        n = std::string_view("4563 ");
        REQUIRE(n == 4563);
        REQUIRE_THROWS_PREDICATE(n = std::string_view(""), std::invalid_argument, [](const std::invalid_argument &ia) {
            return std::string(ia.what()) == "The string '' is not a valid integer in base 10";
        });
#endif
    }
};

TEST_CASE("string assignment")
{
    tuple_for_each(sizes{}, string_ass_tester{});
}

struct promdem_tester {
    template <typename S>
    void operator()(const S &) const
    {
        using integer = integer<S::value>;
        integer n;
        REQUIRE(n.promote());
        REQUIRE(n.sgn() == 0);
        REQUIRE(n.is_dynamic());
        REQUIRE(!n.promote());
        REQUIRE(n.demote());
        REQUIRE(n.sgn() == 0);
        REQUIRE(n.is_static());
        REQUIRE(!n.demote());
        n = -5;
        REQUIRE(n.promote());
        REQUIRE(n == -5);
        REQUIRE(n.is_dynamic());
        REQUIRE(!n.promote());
        REQUIRE(n.demote());
        REQUIRE(n == -5);
        REQUIRE(n.is_static());
        REQUIRE(!n.demote());
        n = integer{"312321983721983791287392817328917398217398712938719273981273"};
        if (n.size() > S::value) {
            REQUIRE(n.is_dynamic());
            REQUIRE(!n.demote());
            REQUIRE(n.is_dynamic());
        }
    }
};

TEST_CASE("promote and demote")
{
    tuple_for_each(sizes{}, promdem_tester{});
}

struct sign_tester {
    template <typename S>
    void operator()(const S &) const
    {
        using integer = integer<S::value>;
        integer n;
        REQUIRE(n.sgn() == 0);
        REQUIRE(sgn(n) == 0);
        n.promote();
        REQUIRE(n.sgn() == 0);
        REQUIRE(sgn(n) == 0);
        n = 12;
        REQUIRE(n.sgn() == 1);
        REQUIRE(sgn(n) == 1);
        n.promote();
        REQUIRE(n.sgn() == 1);
        REQUIRE(sgn(n) == 1);
        n = -34;
        REQUIRE(n.sgn() == -1);
        REQUIRE(sgn(n) == -1);
        n.promote();
        REQUIRE(n.sgn() == -1);
        REQUIRE(sgn(n) == -1);
    }
};

TEST_CASE("sign")
{
    tuple_for_each(sizes{}, sign_tester{});
}

struct to_string_tester {
    template <typename S>
    void operator()(const S &) const
    {
        using integer = integer<S::value>;
        REQUIRE(integer{}.to_string() == "0");
        REQUIRE(integer{1}.to_string() == "1");
        REQUIRE(integer{-1}.to_string() == "-1");
        REQUIRE(integer{123}.to_string() == "123");
        REQUIRE(integer{-123}.to_string() == "-123");
        REQUIRE(integer{123}.to_string(3) == "11120");
        REQUIRE(integer{-123}.to_string(3) == "-11120");
        REQUIRE_THROWS_PREDICATE((integer{}.to_string(1)), std::invalid_argument, [](const std::invalid_argument &ia) {
            return std::string(ia.what())
                   == "Invalid base for string conversion: the base must be between "
                      "2 and 62, but a value of 1 was provided instead";
        });
        REQUIRE_THROWS_PREDICATE((integer{}.to_string(-12)), std::invalid_argument,
                                 [](const std::invalid_argument &ia) {
                                     return std::string(ia.what())
                                            == "Invalid base for string conversion: the base must be between "
                                               "2 and 62, but a value of -12 was provided instead";
                                 });
        REQUIRE_THROWS_PREDICATE((integer{}.to_string(63)), std::invalid_argument, [](const std::invalid_argument &ia) {
            return std::string(ia.what())
                   == "Invalid base for string conversion: the base must be between "
                      "2 and 62, but a value of 63 was provided instead";
        });
    }
};

TEST_CASE("to_string")
{
    tuple_for_each(sizes{}, to_string_tester{});
}

struct stream_tester {
    template <typename S>
    void operator()(const S &) const
    {
        using integer = integer<S::value>;
        {
            std::ostringstream oss;
            oss << integer{};
            REQUIRE(oss.str() == "0");
        }
        {
            std::ostringstream oss;
            oss << integer{123};
            REQUIRE(oss.str() == "123");
        }
        {
            std::ostringstream oss;
            oss << integer{-123};
            REQUIRE(oss.str() == "-123");
        }
        {
            std::stringstream ss;
            ss << integer{};
            integer n(12);
            ss >> n;
            REQUIRE(n == 0);
        }
        {
            std::stringstream ss;
            ss << integer{-123};
            integer n;
            ss >> n;
            REQUIRE(n == -123);
        }
        {
            std::stringstream ss;
            ss.str("-42");
            integer n;
            ss >> n;
            REQUIRE(n == -42);
        }
    }
};

TEST_CASE("stream")
{
    tuple_for_each(sizes{}, stream_tester{});
}

struct yes {
};

struct no {
};

template <typename From, typename To>
static inline auto test_static_cast(int) -> decltype(void(static_cast<To>(std::declval<const From &>())), yes{});

template <typename From, typename To>
static inline no test_static_cast(...);

template <typename From, typename To>
using is_convertible = std::integral_constant<bool, std::is_same<decltype(test_static_cast<From, To>(0)), yes>::value>;

template <typename Integer, typename T>
static inline bool roundtrip_conversion(const T &x)
{
    Integer tmp{x};
    T rop1, rop2;
    bool retval = (static_cast<T>(tmp) == x) && (lex_cast(x) == lex_cast(tmp));
    const bool res1 = tmp.get(rop1);
    const bool res2 = get(rop2, tmp);
    retval = retval && res1 && res2;
    retval = retval && (lex_cast(rop1) == lex_cast(tmp)) && (lex_cast(rop2) == lex_cast(tmp));
    return retval;
}

struct no_conv {
};

struct int_convert_tester {
    template <typename S>
    struct runner {
        template <typename Int>
        void operator()(const Int &) const
        {
            using integer = integer<S::value>;
            REQUIRE((is_convertible<integer, Int>::value));
            REQUIRE(roundtrip_conversion<integer>(0));
            auto constexpr min = nl_min<Int>(), max = nl_max<Int>();
            REQUIRE(roundtrip_conversion<integer>(min));
            REQUIRE(roundtrip_conversion<integer>(max));
            REQUIRE(roundtrip_conversion<integer>(Int(42)));
            REQUIRE(roundtrip_conversion<integer>(Int(-42)));
            REQUIRE(roundtrip_conversion<integer>(min + Int(1)));
            REQUIRE(roundtrip_conversion<integer>(max - Int(1)));
            REQUIRE(roundtrip_conversion<integer>(min + Int(2)));
            REQUIRE(roundtrip_conversion<integer>(max - Int(2)));
            REQUIRE(roundtrip_conversion<integer>(min + Int(3)));
            REQUIRE(roundtrip_conversion<integer>(max - Int(3)));
            REQUIRE(roundtrip_conversion<integer>(min + Int(42)));
            REQUIRE(roundtrip_conversion<integer>(max - Int(42)));
            Int rop(1);
            REQUIRE_THROWS_PREDICATE(static_cast<Int>(integer(min) - 1), std::overflow_error,
                                     [](const std::overflow_error &) { return true; });
            REQUIRE(!(integer(min) - 1).get(rop));
            REQUIRE(!get(rop, integer(min) - 1));
            REQUIRE(rop == Int(1));
            REQUIRE_THROWS_PREDICATE(static_cast<Int>(integer(min) - 2), std::overflow_error,
                                     [](const std::overflow_error &) { return true; });
            REQUIRE(!(integer(min) - 2).get(rop));
            REQUIRE(!get(rop, integer(min) - 2));
            REQUIRE(rop == Int(1));
            REQUIRE_THROWS_PREDICATE(static_cast<Int>(integer(min) - 3), std::overflow_error,
                                     [](const std::overflow_error &) { return true; });
            REQUIRE(!(integer(min) - 3).get(rop));
            REQUIRE(!get(rop, integer(min) - 3));
            REQUIRE(rop == Int(1));
            REQUIRE_THROWS_PREDICATE(static_cast<Int>(integer(min) - 123), std::overflow_error,
                                     [](const std::overflow_error &) { return true; });
            REQUIRE(!(integer(min) - 123).get(rop));
            REQUIRE(!get(rop, integer(min) - 123));
            REQUIRE(rop == Int(1));
            REQUIRE_THROWS_PREDICATE(static_cast<Int>(integer(max) + 1), std::overflow_error,
                                     [](const std::overflow_error &) { return true; });
            REQUIRE(!(integer(max) + 1).get(rop));
            REQUIRE(!get(rop, integer(max) + 1));
            REQUIRE(rop == Int(1));
            REQUIRE_THROWS_PREDICATE(static_cast<Int>(integer(max) + 2), std::overflow_error,
                                     [](const std::overflow_error &) { return true; });
            REQUIRE(!(integer(max) + 2).get(rop));
            REQUIRE(!get(rop, integer(max) + 2));
            REQUIRE(rop == Int(1));
            REQUIRE_THROWS_PREDICATE(static_cast<Int>(integer(max) + 3), std::overflow_error,
                                     [](const std::overflow_error &) { return true; });
            REQUIRE(!(integer(max) + 3).get(rop));
            REQUIRE(!get(rop, integer(max) + 3));
            REQUIRE(rop == Int(1));
            REQUIRE_THROWS_PREDICATE(static_cast<Int>(integer(max) + 123), std::overflow_error,
                                     [](const std::overflow_error &) { return true; });
            REQUIRE(!(integer(max) + 123).get(rop));
            REQUIRE(!get(rop, integer(max) + 123));
            REQUIRE(rop == Int(1));
            // Try with large integers that should trigger a specific error, at least on some platforms.
            REQUIRE_THROWS_PREDICATE(static_cast<Int>(integer(max) * max * max * max * max), std::overflow_error,
                                     [](const std::overflow_error &) { return true; });
            REQUIRE(!(integer(max) * max * max * max * max).get(rop));
            REQUIRE(!get(rop, integer(max) * max * max * max * max));
            REQUIRE(rop == Int(1));
            if (min != Int(0)) {
                REQUIRE_THROWS_PREDICATE(static_cast<Int>(integer(min) * min * min * min * min), std::overflow_error,
                                         [](const std::overflow_error &) { return true; });
                REQUIRE(!(integer(min) * min * min * min * min).get(rop));
                REQUIRE(!get(rop, integer(min) * min * min * min * min));
                REQUIRE(rop == Int(1));
            }
            std::atomic<bool> fail(false);
            auto f = [&fail](unsigned n) {
                integral_minmax_dist<Int> dist;
                std::mt19937 eng(static_cast<std::mt19937::result_type>(n + mt_rng_seed));
                for (auto i = 0; i < ntries; ++i) {
                    if (!roundtrip_conversion<integer>(static_cast<Int>(dist(eng)))) {
                        fail.store(false);
                    }
                }
            };
            std::thread t0(f, 0u), t1(f, 1u), t2(f, 2u), t3(f, 3u);
            t0.join();
            t1.join();
            t2.join();
            t3.join();
            REQUIRE(!fail.load());
            mt_rng_seed += 4u;
        }
    };
    template <typename S>
    inline void operator()(const S &) const
    {
        tuple_for_each(int_types{}, runner<S>{});
        // Some testing for bool.
        using integer = integer<S::value>;
        REQUIRE((is_convertible<integer, bool>::value));
        REQUIRE(roundtrip_conversion<integer>(true));
        REQUIRE(roundtrip_conversion<integer>(false));
        // Extra.
        REQUIRE((!is_convertible<integer, no_conv>::value));
    }
};

TEST_CASE("integral conversions")
{
    tuple_for_each(sizes{}, int_convert_tester{});
}

struct fp_convert_tester {
    template <typename S>
    struct runner {
        template <typename Float>
        void operator()(const Float &) const
        {
            using integer = integer<S::value>;
            REQUIRE((is_convertible<integer, Float>::value));
            Float rop(1);
            REQUIRE(static_cast<Float>(integer{0}) == Float(0));
            REQUIRE(integer{0}.get(rop));
            REQUIRE(get(rop, integer{0}));
            REQUIRE(rop == Float(0));
            REQUIRE(static_cast<Float>(integer{1}) == Float(1));
            REQUIRE(integer{1}.get(rop));
            REQUIRE(get(rop, integer{1}));
            REQUIRE(rop == Float(1));
            REQUIRE(static_cast<Float>(integer{-1}) == Float(-1));
            REQUIRE(integer{-1}.get(rop));
            REQUIRE(get(rop, integer{-1}));
            REQUIRE(rop == Float(-1));
            REQUIRE(static_cast<Float>(integer{12}) == Float(12));
            REQUIRE(integer{12}.get(rop));
            REQUIRE(get(rop, integer{12}));
            REQUIRE(rop == Float(12));
            REQUIRE(static_cast<Float>(integer{-12}) == Float(-12));
            REQUIRE(integer{-12}.get(rop));
            REQUIRE(get(rop, integer{-12}));
            REQUIRE(rop == Float(-12));
            if (std::numeric_limits<Float>::is_iec559) {
                // Try with large numbers.
                REQUIRE(std::abs(static_cast<Float>(integer{"1000000000000000000000000000000"}) - Float(1E30))
                            / Float(1E30)
                        <= std::numeric_limits<Float>::epsilon() * 1000.);
                REQUIRE(std::abs(static_cast<Float>(integer{"-1000000000000000000000000000000"}) + Float(1E30))
                            / Float(1E30)
                        <= std::numeric_limits<Float>::epsilon() * 1000.);
                REQUIRE(static_cast<Float>(integer{std::numeric_limits<Float>::max()})
                        == std::numeric_limits<Float>::max());
                REQUIRE(static_cast<Float>(integer{-std::numeric_limits<Float>::max()})
                        == -std::numeric_limits<Float>::max());
            }
            // Random testing.
            std::atomic<bool> fail(false);
            auto f = [&fail](unsigned n) {
                std::uniform_real_distribution<Float> dist(Float(-100), Float(100));
                std::mt19937 eng(static_cast<std::mt19937::result_type>(n + mt_rng_seed));
                for (auto i = 0; i < ntries; ++i) {
                    Float rop1;
                    const auto tmp = dist(eng);
                    if (static_cast<Float>(integer{tmp}) != std::trunc(tmp)) {
                        fail.store(false);
                    }
                    if (!integer{tmp}.get(rop1)) {
                        fail.store(false);
                    }
                    if (!get(rop1, integer{tmp})) {
                        fail.store(false);
                    }
                    if (rop1 != std::trunc(tmp)) {
                        fail.store(false);
                    }
                }
            };
            std::thread t0(f, 0u), t1(f, 1u), t2(f, 2u), t3(f, 3u);
            t0.join();
            t1.join();
            t2.join();
            t3.join();
            REQUIRE(!fail.load());
            mt_rng_seed += 4u;
        }
    };
    template <typename S>
    inline void operator()(const S &) const
    {
        tuple_for_each(fp_types{}, runner<S>{});
    }
};

TEST_CASE("floating-point conversions")
{
    tuple_for_each(sizes{}, fp_convert_tester{});
}

struct sizes_tester {
    template <typename S>
    void operator()(const S &) const
    {
        using integer = integer<S::value>;
        integer n;
        REQUIRE(n.nbits() == 0u);
        REQUIRE(n.size() == 0u);
        n = 1;
        REQUIRE(n.nbits() == 1u);
        REQUIRE(n.size() == 1u);
        n = -1;
        REQUIRE(n.nbits() == 1u);
        REQUIRE(n.size() == 1u);
        n = 3;
        REQUIRE(n.nbits() == 2u);
        REQUIRE(n.size() == 1u);
        n = -3;
        REQUIRE(n.nbits() == 2u);
        REQUIRE(n.size() == 1u);
        n = 1;
        n <<= GMP_NUMB_BITS;
        REQUIRE(n.nbits() == GMP_NUMB_BITS + 1);
        REQUIRE(n.size() == 2u);
        n = -1;
        n <<= GMP_NUMB_BITS;
        REQUIRE(n.nbits() == GMP_NUMB_BITS + 1);
        REQUIRE(n.size() == 2u);
        // Static data member.
        REQUIRE(integer::ssize == S::value);
        // Random testing.
        mpz_raii tmp;
        std::uniform_int_distribution<int> sdist(0, 1);
        auto random_x = [&](unsigned x) {
            for (int i = 0; i < ntries; ++i) {
                random_integer(tmp, x, rng);
                n = &tmp.m_mpz;
                if (n.is_static() && sdist(rng)) {
                    // Promote sometimes, if possible.
                    n.promote();
                }
                const auto res1 = n.nbits();
                const auto res2 = n.sgn() ? ::mpz_sizeinbase(&tmp.m_mpz, 2) : 0u;
                REQUIRE(res1 == res2);
            }
        };
        random_x(0);
        random_x(1);
        random_x(2);
        random_x(3);
        random_x(4);
    }
};

TEST_CASE("sizes")
{
    tuple_for_each(sizes{}, sizes_tester{});
}
