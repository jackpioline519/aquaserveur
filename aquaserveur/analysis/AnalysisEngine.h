#pragma once

#include "../models/ConfigModels.h"
#include "../models/RuntimeModels.h"
#include "../models/AnalysisModels.h"
#include "../support/LogManager.h"
#include "ManualTestManager.h"
#include "../sensors/SensorManager.h"
#include <vector>

namespace Aqua {

class AnalysisEngine {
public:
    AnalysisEngine(LogManager& logger, const SensorManager& sensorManager, const ManualTestManager& manualTestManager);

    void run(const ServerConfig& config,
             const std::vector<RuntimeSensorValue>& sensorValues,
             const std::vector<ManualTestValue>& manualTests,
             std::vector<AnalysisResult>& results);

private:
    bool resolveVariable(const String& variable,
                         const std::vector<RuntimeSensorValue>& sensorValues,
                         const std::vector<ManualTestValue>& manualTests,
                         double& outValue) const;
    bool evaluateCondition(const String& condition, double left, double right) const;

    LogManager& m_logger;
    const SensorManager& m_sensorManager;
    const ManualTestManager& m_manualTestManager;
};

} // namespace Aqua
