#include "ConfigManager.h"

namespace Aqua {

ConfigManager::ConfigManager(ConfigLoader& loader, ConfigValidator& validator, LogManager& logger)
    : m_loader(loader), m_validator(validator), m_logger(logger) {}

bool ConfigManager::loadInitialConfig(const String& path, SystemContext& context, String& outError) {
    ServerConfig config;
    if (!m_loader.loadFromFile(path, config, outError)) {
        m_logger.error("ConfigManager", outError);
        return false;
    }

    std::vector<String> errors;
    std::vector<String> warnings;
    if (!m_validator.validate(config, errors, warnings)) {
        outError = errors.empty() ? "Configuration invalide" : errors.front();
        for (const auto& e : errors) m_logger.error("ConfigValidator", e);
        for (const auto& w : warnings) m_logger.warn("ConfigValidator", w);
        return false;
    }

    for (const auto& w : warnings) m_logger.warn("ConfigValidator", w);
    context.activeConfig = config;
    context.systemState.configLoaded = true;
    context.systemState.configName = config.system.name;
    context.systemState.configVersion = config.system.configVersion;
    m_logger.info("ConfigManager", "Configuration initiale chargée: " + config.system.name);
    return true;
}

bool ConfigManager::replaceConfigFromJson(const String& json, ServerConfig& outConfig, String& outError) {
    if (!m_loader.loadFromJsonString(json, outConfig, outError)) {
        m_logger.error("ConfigManager", outError);
        return false;
    }

    std::vector<String> errors;
    std::vector<String> warnings;
    if (!m_validator.validate(outConfig, errors, warnings)) {
        outError = errors.empty() ? "Nouvelle configuration invalide" : errors.front();
        for (const auto& e : errors) m_logger.error("ConfigValidator", e);
        for (const auto& w : warnings) m_logger.warn("ConfigValidator", w);
        return false;
    }

    for (const auto& w : warnings) m_logger.warn("ConfigValidator", w);
    m_logger.info("ConfigManager", "Nouvelle configuration validée");
    return true;
}

} // namespace Aqua
