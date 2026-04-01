#pragma once

#include "../models/ConfigModels.h"
#include "../models/RuntimeModels.h"
#include "../support/LogManager.h"
#include <vector>

namespace Aqua {

class OutputManager {
public:
    explicit OutputManager(LogManager& logger);

    void initialize(const std::vector<OutputDefinition>& outputs, std::vector<OutputState>& states);
    bool applyOutputCommand(const std::vector<OutputDefinition>& outputs,
                            std::vector<OutputState>& states,
                            const String& outputId,
                            bool newState,
                            bool isManualOverride,
                            uint32_t overrideDurationMs,
                            String& outError);
    bool findState(const std::vector<OutputState>& states, const String& outputId, OutputState*& outState);
    void releaseExpiredOverrides(std::vector<OutputState>& states);

private:
    LogManager& m_logger;
};

} // namespace Aqua
