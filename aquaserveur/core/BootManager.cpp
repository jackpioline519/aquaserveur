#include "BootManager.h"

namespace Aqua {

BootManager::BootManager(ConfigManager& configManager,
                         SensorManager& sensorManager,
                         OutputManager& outputManager,
                         LogManager& logger)
    : m_configManager(configManager),
      m_sensorManager(sensorManager),
      m_outputManager(outputManager),
      m_logger(logger) {}

bool BootManager::boot(const String& configPath, SystemContext& context, String& outError) {
    context.clear();

    if (!m_configManager.loadInitialConfig(configPath, context, outError)) {
        context.systemState.degradedMode = true;
        return false;
    }

    return rebuildFromActiveConfig(context, outError);
}

bool BootManager::rebuildFromActiveConfig(SystemContext& context, String& outError) {
    (void)outError;
    m_sensorManager.initialize(context.activeConfig.sensors, context.sensorValues);
    m_outputManager.initialize(context.activeConfig.outputs, context.outputStates);
    context.manualTests.clear();
    context.analysisResults.clear();
    context.adviceResults.clear();
    context.systemState.degradedMode = false;
    m_logger.info("BootManager", "Modèle runtime reconstruit depuis la configuration active");
    return true;
}

} // namespace Aqua
