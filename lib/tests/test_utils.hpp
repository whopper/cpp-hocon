#pragma once

#include <internal/simple_config_origin.hpp>

#include <string>

namespace hocon {

    std::unique_ptr<simple_config_origin> fake_origin(std::string description = "fake");

}  // namespace hocon
