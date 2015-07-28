#include "test_utils.hpp"

using namespace std;

namespace hocon {

    unique_ptr<simple_config_origin> fake_origin(string description) {
        return unique_ptr<simple_config_origin>(new simple_config_origin(move(description)));
    }

}  // namespace hocon
