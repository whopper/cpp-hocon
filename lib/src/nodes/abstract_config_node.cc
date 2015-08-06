#include <internal/abstract_config_node.hpp>

using namespace std;

namespace hocon {
    string abstract_config_node::render() const {
        string orig_text;
        for (auto it = get_tokens().begin(); it != get_tokens().end(); ++it) {
            orig_text.append(it->token_text());
        }
        return orig_text;
    }

    bool abstract_config_node::operator==(const abstract_config_node &other) const {
        return (render() == other.render());
    }

}  // namespace hocon
