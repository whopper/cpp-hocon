#include <catch.hpp>

#include <internal/token.hpp>
#include <internal/tokens.hpp>
#include <internal/simple_config_origin.hpp>
#include "test_utils.hpp"

using namespace hocon;

TEST_CASE("token equality", "[tokens]") {

    SECTION("problem token equality") {
        problem problem_token(fake_origin(), "test exception", "it broke", false);
        problem other_problem(fake_origin(), "test exception", "it broke", false);
        ignored_whitespace not_a_problem(fake_origin(), "   ");

        REQUIRE(problem_token == other_problem);
        REQUIRE_FALSE(problem_token == not_a_problem);
    }
}
