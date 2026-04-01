#pragma once

#include "../models/ConfigModels.h"
#include "../models/RuntimeModels.h"
#include "../models/AnalysisModels.h"
#include <vector>

namespace Aqua {

class SystemContext {
public:
    void clear();

    ServerConfig activeConfig;
    std::vector<RuntimeSensorValue> sensorValues;
    std::vector<ManualTestValue> manualTests;
    std::vector<OutputState> outputStates;
    std::vector<AnalysisResult> analysisResults;
    std::vector<AdviceResult> adviceResults;
    SystemState systemState;
};

} // namespace Aqua
