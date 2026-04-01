#pragma once

#include "../models/CommandModels.h"
#include "../core/SystemContext.h"
#include "../support/LogManager.h"
#include "../outputs/OutputManager.h"
#include "../analysis/ManualTestManager.h"
#include "../config/ConfigManager.h"

namespace Aqua {

class CommandManager {
public:
    CommandManager(LogManager& logger,
                   OutputManager& outputManager,
                   ManualTestManager& manualTestManager,
                   ConfigManager& configManager);

    bool decode(const String& rawJson, IhmCommand& outCommand, String& outError) const;
    CommandResult process(const IhmCommand& command, SystemContext& context);
    bool consumePendingConfig(SystemContext& context, bool& outChanged);

private:
    LogManager& m_logger;
    OutputManager& m_outputManager;
    ManualTestManager& m_manualTestManager;
    ConfigManager& m_configManager;

    bool m_hasPendingConfig = false;
    ServerConfig m_pendingConfig;
};

} // namespace Aqua
