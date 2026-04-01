#include "CommandManager.h"
#include <ArduinoJson.h>

namespace Aqua {

CommandManager::CommandManager(LogManager& logger,
                               OutputManager& outputManager,
                               ManualTestManager& manualTestManager,
                               ConfigManager& configManager)
    : m_logger(logger),
      m_outputManager(outputManager),
      m_manualTestManager(manualTestManager),
      m_configManager(configManager) {}

bool CommandManager::decode(const String& rawJson, IhmCommand& outCommand, String& outError) const {
    JsonDocument doc;
    auto err = deserializeJson(doc, rawJson);
    if (err) {
        outError = String("Commande JSON invalide: ") + err.c_str();
        return false;
    }

    const String type = doc["type"] | "";
    outCommand.targetId = doc["targetId"] | "";
    outCommand.payload = doc["payload"] | "";
    outCommand.boolValue = doc["boolValue"] | false;
    outCommand.numericValue = doc["numericValue"].isNull() ? NAN : static_cast<double>(doc["numericValue"].as<double>());
    outCommand.timestampMs = millis();

    if (type == "set_output") outCommand.type = IhmCommandType::SetOutput;
    else if (type == "add_manual_test") outCommand.type = IhmCommandType::AddManualTest;
    else if (type == "replace_config") outCommand.type = IhmCommandType::ReplaceConfig;
    else if (type == "get_state") outCommand.type = IhmCommandType::GetState;
    else outCommand.type = IhmCommandType::Unknown;

    if (outCommand.type == IhmCommandType::Unknown) {
        outError = "Type de commande inconnu";
        return false;
    }

    return true;
}

CommandResult CommandManager::process(const IhmCommand& command, SystemContext& context) {
    CommandResult result;
    String error;

    switch (command.type) {
        case IhmCommandType::SetOutput: {
            result.success = m_outputManager.applyOutputCommand(
                context.activeConfig.outputs,
                context.outputStates,
                command.targetId,
                command.boolValue,
                true,
                30UL * 60UL * 1000UL,
                error);
            result.message = result.success ? "Commande sortie appliquée" : error;
            break;
        }

        case IhmCommandType::AddManualTest: {
            result.success = m_manualTestManager.storeTest(
                context.activeConfig.manualTests,
                context.manualTests,
                command.targetId,
                command.numericValue,
                error);
            result.message = result.success ? "Test manuel enregistré" : error;
            break;
        }

        case IhmCommandType::ReplaceConfig: {
            ServerConfig newConfig;
            result.success = m_configManager.replaceConfigFromJson(command.payload, newConfig, error);
            if (result.success) {
                m_pendingConfig = newConfig;
                m_hasPendingConfig = true;
                result.message = "Nouvelle configuration validée et en attente d'application";
            } else {
                result.message = error;
            }
            break;
        }

        case IhmCommandType::GetState: {
            result.success = true;
            result.message = "Demande d'état acceptée";
            break;
        }

        default:
            result.success = false;
            result.message = "Commande non gérée";
            break;
    }

    if (result.success) m_logger.info("CommandManager", result.message);
    else m_logger.warn("CommandManager", result.message);

    return result;
}

bool CommandManager::consumePendingConfig(SystemContext& context, bool& outChanged) {
    outChanged = false;
    if (!m_hasPendingConfig) {
        return true;
    }

    context.activeConfig = m_pendingConfig;
    context.systemState.configName = m_pendingConfig.system.name;
    context.systemState.configVersion = m_pendingConfig.system.configVersion;
    context.systemState.configLoaded = true;

    m_hasPendingConfig = false;
    outChanged = true;
    m_logger.info("CommandManager", "Nouvelle configuration activée");
    return true;
}

} // namespace Aqua
