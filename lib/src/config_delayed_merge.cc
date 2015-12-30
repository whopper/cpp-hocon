#include <internal/config_delayed_merge.hpp>

using namespace std;

namespace hocon {

    config_delayed_merge::config_delayed_merge(shared_origin origin, std::vector<shared_value> stack) :
        config_value(move(origin)), _stack(stack) {

        if (_stack.empty()) {
            throw config_exception("creating empty delayed merge value");
        }

        for (auto&& v: stack) {
            if (dynamic_pointer_cast<const config_delayed_merge>(v) || dynamic_pointer_cast<const config_delayed_merge_object>(v)) {
                throw config_exception("placed nested delayed_merge in a config_delayed_merge, should have consolidated stack");
            }
        }
    }

    config_value_type config_delayed_merge::value_type() const {
        throw config_exception("called valueType() on value with unresolved substitutions, need to Config#resolve() first, see API docs");
    }

    vector<shared_value> config_delayed_merge::unmerged_values() {
        return _stack;
    }
}  // namespace hocon::config_delayed_merge