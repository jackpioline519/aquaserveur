#pragma once

#include "../models/ConfigModels.h"

namespace Aqua {

class ConfigLoader {
public:
    bool loadFromFile(const String& path, ServerConfig& outConfig, String& outError) const;
    bool loadFromJsonString(const String& json, ServerConfig& outConfig, String& outError) const;
};

} // namespace Aqua
