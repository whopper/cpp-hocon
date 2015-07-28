#pragma once

#include <hocon/config_value.hpp>
#include <internal/simple_config_origin.hpp>

namespace hocon {

    enum class resolved_status { RESOLVED, UNRESOLVED };

    class abstract_config_value : public config_value {
    public:
        abstract_config_value(simple_config_origin origin);

        virtual std::string transform_to_string() const;

        virtual resolved_status resolved() const;

        simple_config_origin const& origin() const;

    private:
        simple_config_origin _origin;
    };

}  // namespace hocon
