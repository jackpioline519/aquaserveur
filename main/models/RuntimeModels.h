#pragma once

#include <map>
#include <string>

namespace Aqua {

    struct SensorValue {
        double value = 0.0;
        bool valid = false;
        long timestamp = 0;
    };

    struct OutputState {
        bool enabled = false;
        std::string state = "off";
    };

    struct AnalysisResult {
        std::string resultCode;
        std::string severity;
        std::string message;
    };

    struct RuntimeState {
        std::map<std::string, SensorValue> sensorValues;
        std::map<std::string, double> manualTestValues;
        std::map<std::string, OutputState> outputs;
    };

}