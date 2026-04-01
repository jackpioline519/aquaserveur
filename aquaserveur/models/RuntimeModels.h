#pragma once

#include <Arduino.h>
#include <vector>

namespace Aqua {

struct RuntimeSensorValue {
    String sensorId;
    String role;
    double rawValue = NAN;
    double normalizedValue = NAN;
    uint64_t timestampMs = 0;
    bool valid = false;
    String quality = "unknown";
};

struct ManualTestValue {
    String testId;
    String role;
    double value = NAN;
    uint64_t timestampMs = 0;
    uint64_t expiresAtMs = 0;
    bool valid = false;
};

struct OutputState {
    String outputId;
    bool currentState = false;
    bool blocked = false;
    bool overridden = false;
    uint64_t overrideUntilMs = 0;
    String source = "default";
};

struct SystemState {
    String configName;
    String configVersion;
    bool configLoaded = false;
    bool degradedMode = false;
    uint64_t lastPublishMs = 0;
    size_t sensorCount = 0;
    size_t activeAlertCount = 0;
};

} // namespace Aqua
