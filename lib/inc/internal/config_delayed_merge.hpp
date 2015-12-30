#include <hocon/config_value.hpp>
#include <internal/config_delayed_merge_object.hpp>
#include <internal/unmergeable.hpp>
#include <internal/config_exception.hpp>
#include <vector>

#pragma once

namespace hocon {

    class config_delayed_merge : public config_value, public unmergeable {
    public:
        config_delayed_merge(shared_origin origin, std::vector<shared_value> stack);

        config_value_type value_type() const override;
        std::vector<shared_value> unmerged_values() override;

    private:
        std::vector<shared_value> _stack;
    };

}  // namespace hocon::config_delayed_merge

