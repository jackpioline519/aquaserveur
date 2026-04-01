#pragma once

#include <Arduino.h>

namespace Aqua {

enum class IhmCommandType {
    Unknown,
    SetOutput,
    AddManualTest,
    ReplaceConfig,
    GetState
};

struct IhmCommand {
    IhmCommandType type = IhmCommandType::Unknown;
    String targetId;
    String payload;
    bool boolValue = false;
    double numericValue = NAN;
    uint64_t timestampMs = 0;
};

struct CommandResult {
    bool success = false;
    String message;
};

} // namespace Aqua
