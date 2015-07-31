#include <catch.hpp>

#include "test_utils.hpp"
#include <internal/simple_config_origin.hpp>
#include <internal/values/config_long.hpp>
#include <internal/values/config_double.hpp>

using namespace hocon;

TEST_CASE("simple_config_origin equality", "[config_values]") {
    auto org1 = simple_config_origin("foo");
    auto org2 = simple_config_origin("foo");
    auto org3 = simple_config_origin("bar");

    SECTION("different origins with the same name should be equal") {
        REQUIRE(org1 == org2);
        REQUIRE(org1 != org3);
    }
}

TEST_CASE("config_number equality", "[config_values]") {

    SECTION("config_long equality") {
        config_long a { fake_origin("fake"), 3, "3" };
        config_long a2 { fake_origin("fake"), 3, "3" };
        config_long b { fake_origin("fake"), 4, "4" };

        REQUIRE(a == a2);
        REQUIRE(a != b);
    }

    SECTION("config_double equality") {
        config_double a { fake_origin("fake"), 3.0, "3.0" };
        config_double a2 { fake_origin("fake"), 3.0, "3.0" };
        config_double b { fake_origin("fake"), 4.5, "4.5" };

        REQUIRE(a == a2);
        REQUIRE(a != b);
    }

    SECTION("Int can equal double") {
        config_long an_int { fake_origin("fake"), 3, "3" };
        config_double a_double { fake_origin("fake"), 3.0, "3.0" };
        config_double another_double { fake_origin("fake"), 3.5, "3.5" };

        REQUIRE(an_int == a_double);
        REQUIRE_FALSE(an_int == another_double);
    }
}
