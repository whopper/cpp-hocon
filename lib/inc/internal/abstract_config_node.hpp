#pragma once

#include <hocon/config_node.hpp>
#include <internal/token.hpp>
#include <vector>

namespace hocon {
    class abstract_config_node : public config_node {
        public:
            std::string render() const;
            bool operator==(const abstract_config_node &other) const;
            virtual std::vector<token> get_tokens() const = 0;
    };

}  // namespace hocon
