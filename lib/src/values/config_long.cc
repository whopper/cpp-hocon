#include <internal/values/config_long.hpp>

using namespace std;

namespace hocon {

    config_long::config_long(shared_ptr<simple_config_origin> origin, long value, string original_text) :
            config_number(origin, move(original_text)), _value(value) { }

    std::string config_long::transform_to_string() const {
        string s = config_number::transform_to_string();
        if (s.empty()) {
            return to_string(_value);
        } else {
            return s;
        }
    }

    long config_long::long_value() const {
        return _value;
    }

    double config_long::double_value() const {
        return _value;
    }

}  // namespace hocon