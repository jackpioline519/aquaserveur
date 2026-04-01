

#pragma once

#include <cmath>
#include <string>
#include <vector>

namespace Aqua {

    struct SystemInfo {
        std::string name;
        std::string configVersion;
    };

    struct SensorDefinition {
        std::string id;
        std::string type;
        std::string protocol;
        int pin = -1;
        std::string unit;
        std::string role;
    };

    struct OutputDefinition {
        std::string id;
        int pin = -1;
        std::string role;
        std::string label;
        std::string defaultState = "off";
    };

    struct ManualTestDefinition {
        std::string id;
        std::string label;
        std::string unit;
        std::string role;
        double minValue = NAN;
        double maxValue = NAN;
    };

    struct AnalysisRuleDefinition {
        std::string id;
        std::string variable;
        std::string condition;
        double threshold = 0.0;
        std::string severity;
        std::string resultCode;
        std::string message;
    };

    struct AdviceDefinition {
        std::string resultCode;
        std::string preventive;
        std::string corrective;
    };

    struct RoutineDefinition {
        std::string id;
        std::string outputId;
        std::string start;
        std::string end;
        std::vector<std::string> days;
    };

    struct CommConfig {
        int port = 80;
        std::string protocol = "websocket";
    };

    struct LogConfig {
        bool enableSerial = true;
        bool enableSd = false;
        std::string level = "info";
    };

    struct ServerConfig {
        SystemInfo system;
        std::vector<SensorDefinition> sensors;
        std::vector<OutputDefinition> outputs;
        std::vector<ManualTestDefinition> manualTests;
        std::vector<AnalysisRuleDefinition> analysisRules;
        std::vector<AdviceDefinition> advices;
        std::vector<RoutineDefinition> routines;
        CommConfig communication;
        LogConfig logs;
    };

}