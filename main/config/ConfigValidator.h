#pragma once

#include "../models/ConfigModels.h"
#include <vector>

namespace Aqua {

class ConfigValidator {
public:
    bool validate(const ServerConfig& config, std::vector<String>& errors, std::vector<String>& warnings) const;
};

} // namespace Aqua
