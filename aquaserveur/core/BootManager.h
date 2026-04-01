#pragma once

#include "SystemContext.h"
#include "../config/ConfigManager.h"
#include "../sensors/SensorManager.h"
#include "../outputs/OutputManager.h"
#include "../support/LogManager.h"

namespace Aqua {

class BootManager {
public:
    BootManager(ConfigManager& configManager,
                SensorManager& sensorManager,
                OutputManager& outputManager,
                LogManager& logger);

    bool boot(const String& configPath, SystemContext& context, String& outError);
    bool rebuildFromActiveConfig(SystemContext& context, String& outError);

private:
    ConfigManager& m_configManager;
    SensorManager& m_sensorManager;
    OutputManager& m_outputManager;
    LogManager& m_logger;
};

} // namespace Aqua
