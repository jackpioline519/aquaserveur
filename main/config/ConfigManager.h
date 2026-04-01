#pragma once

#include "ConfigLoader.h"
#include "ConfigValidator.h"
#include "../support/LogManager.h"
#include "../core/SystemContext.h"

namespace Aqua {

class ConfigManager {
public:
    ConfigManager(ConfigLoader& loader, ConfigValidator& validator, LogManager& logger);

    bool loadInitialConfig(const String& path, SystemContext& context, String& outError);
    bool replaceConfigFromJson(const String& json, ServerConfig& outConfig, String& outError);

private:
    ConfigLoader& m_loader;
    ConfigValidator& m_validator;
    LogManager& m_logger;
};

} // namespace Aqua
