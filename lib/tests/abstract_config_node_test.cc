#include <catch.hpp>

#include <internal/abstract_config_node.hpp>
#include <internal/token.hpp>

using namespace std;
using namespace hocon;

/*  This test implementation of abstract_config_node allows
 *  shallow testing of its basic methods.
 */
class concrete_config_node : public abstract_config_node {
    public:
        std::vector<token> get_tokens() const { return _tokens; };
        std::vector<token> _tokens;
};

TEST_CASE("abstract_config_node") {
    concrete_config_node node;
    auto org = simple_config_origin("foo");
    token token1(token_type::COMMENT, make_shared<simple_config_origin>(org), "token1");
    token token2(token_type::COMMENT, make_shared<simple_config_origin>(org), "token2");

    SECTION("render should return all tokens concatenated together") {
        node._tokens.push_back(token1);
        node._tokens.push_back(token2);
        REQUIRE(node.render() == "token1token2");
    }

    SECTION("render should return an empty string when there are no tokens") {
        REQUIRE(node.render() == "");
    }

    SECTION("== operator when objects are equivalent") {
        concrete_config_node another_node;
        node._tokens.push_back(token1);
        another_node._tokens.push_back(token1);
        REQUIRE((node == another_node) == true);
    }

    SECTION("== operator when objects are not equivalent") {
        concrete_config_node another_node;
        node._tokens.push_back(token1);
        another_node._tokens.push_back(token2);
        REQUIRE((node == another_node) == false);
    }
}
