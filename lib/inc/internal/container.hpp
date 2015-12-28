#pragma once

#include <hocon/config_value.hpp>

namespace hocon {

    class container : public config_value {
    public:
        /**
         * Replace a child of this value. CAUTION if replacement is null, delete the
         * child, which may also delete the parent, or make the parent into a
         * non-container.
         */
        virtual shared_value replace_child(shared_value child, shared_value replacement) = 0;
        virtual bool has_descendant(shared_value descendant) const = 0;
    };
}