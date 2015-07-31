#include "test_utils.hpp"

using namespace std;

namespace hocon {

    shared_ptr<simple_config_origin> fake_origin(string description) {
        return make_shared<simple_config_origin>(move(description));
    }

}  // namespace hocon
