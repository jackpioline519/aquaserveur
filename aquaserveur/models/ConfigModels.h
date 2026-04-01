#pragma once

#include <Arduino.h>
#include <vector>

namespace Aqua {

struct SystemInfo {
    String name;
    String configVersion;
};

struct SensorDefinition {
    String id;
    String type;
    String protocol;
    int pin = -1;
    String unit;
    String role;
    uint32_t readIntervalMs = 5000;
    bool enabled = true;
};

struct OutputDefinition {
    String id;
    int pin = -1;
    String role;
    String label;
    String defaultState = "off";
    bool manualControlAllowed = false;
    bool enabled = true;
};

struct ManualTestDefinition {
    String id;
    String label;
    String unit;
    String role;
    double minValue = NAN;
    double maxValue = NAN;
    uint32_t validityMs = 24UL * 60UL * 60UL * 1000UL;
};

struct AnalysisRuleDefinition {
    String id;
    String variable;
    String condition;
    double value = 0.0;
    String severity;
    String resultCode;
    String message;
    bool enabled = true;
};

struct AdviceDefinition {
    String resultCode;
    String preventive;
    String corrective;
};

struct RoutineDefinition {
    String id;
    String outputId;
    String start;
    String end;
    std::vector<String> days;
    int priority = 0;
    bool enabled = true;
};

struct IhmConfig {
    String protocol = "websocket";
    uint16_t port = 8080;
    uint32_t publishIntervalMs = 1000;
};

struct LoggingConfig {
    String level = "info";
    size_t maxEntries = 1000;
};

struct ServerConfig {
    SystemInfo system;
    std::vector<SensorDefinition> sensors;
    std::vector<OutputDefinition> outputs;
    std::vector<ManualTestDefinition> manualTests;
    std::vector<AnalysisRuleDefinition> analysisRules;
    std::vector<AdviceDefinition> advice;
    std::vector<RoutineDefinition> routines;
    IhmConfig ihm;
    LoggingConfig logging;
};

} // namespace Aqua
