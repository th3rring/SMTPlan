// Copyright 2016-2018 Francesco Biscani (bluescarni@gmail.com)
//
// This file is part of the mp++ library.
//
// This Source Code Form is subject to the terms of the Mozilla
// Public License v. 2.0. If a copy of the MPL was not distributed
// with this file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <mp++/real128.hpp>

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

using namespace mppp;

TEST_CASE("real128 sincos")
{
    REQUIRE(cos(real128{}) == 1);
    REQUIRE(sin(real128{}) == 0);
    real128 x;
    REQUIRE(x.cos() == 1);
    REQUIRE(x.sin() != 0);
    x = 0;
    REQUIRE(x.sin() == 0);
    REQUIRE(abs(sin(real128{"1.234"}) - real128{"0.943818209374633704861751006156827573"}) < 1E-34);
    REQUIRE(abs(cos(real128{"1.234"}) - real128{"0.330465108071729857403280772789927239"}) < 1E-34);
}
