#pragma once

#include "../models/ConfigModels.h"
#include "../models/RuntimeModels.h"
#include "../support/LogManager.h"
#include "../support/TimeService.h"
#include "../outputs/OutputManager.h"
#include <vector>

namespace Aqua {

class RoutineManager {
public:
    RoutineManager(LogManager& logger, TimeService& timeService, OutputManager& outputManager);

    void apply(const ServerConfig& config, std::vector<OutputState>& states);

private:
    LogManager& m_logger;
    TimeService& m_timeService;
    OutputManager& m_outputManager;
};

} // namespace Aqua
