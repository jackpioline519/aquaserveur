#pragma once

#include "../models/ConfigModels.h"
#include "../models/RuntimeModels.h"
#include "../support/LogManager.h"
#include <vector>

namespace Aqua {

class SensorManager {
public:
    explicit SensorManager(LogManager& logger);

    void initialize(const std::vector<SensorDefinition>& sensors, std::vector<RuntimeSensorValue>& runtimeValues);
    void readSensors(const std::vector<SensorDefinition>& sensors, std::vector<RuntimeSensorValue>& runtimeValues);
    bool getValueByRole(const std::vector<RuntimeSensorValue>& runtimeValues, const String& role, double& outValue) const;

private:
    double readMockSensor(const SensorDefinition& sensor) const;
    LogManager& m_logger;
};

} // namespace Aqua
