#pragma once

#include <Arduino.h>
#include <vector>

namespace Aqua {

struct AnalysisResult {
    String ruleId;
    String resultCode;
    String severity;
    String variable;
    bool triggered = false;
    String message;
};

struct AdviceResult {
    String resultCode;
    String preventive;
    String corrective;
};

} // namespace Aqua
