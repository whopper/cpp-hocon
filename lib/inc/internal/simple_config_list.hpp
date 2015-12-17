#pragma once

#include <hocon/config_list.hpp>
#include <hocon/config_origin.hpp>
#include <vector>

namespace hocon {

    class simple_config_list : public config_value {
    public:
        simple_config_list(shared_origin origin, std::vector<shared_value> value);
        simple_config_list(shared_origin origin, std::vector<shared_value> value, resolve_status status);

        config_value_type value_type() const override;
        resolve_status get_resolve_status() const override;
        std::shared_ptr<simple_config_list> replace_child(shared_value child, shared_value replacement);

    private:
        static const long _serial_version_uid = 2L;
        const std::vector<shared_value> _value;
        const bool _resolved;
    };

}  // namespace hocon


