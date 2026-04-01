#include "StateManager.h"
#include <ArduinoJson.h>

namespace Aqua {

void StateManager::compute(SystemContext& context) const {
    context.systemState.sensorCount = context.sensorValues.size();
    context.systemState.activeAlertCount = 0;

    for (const auto& result : context.analysisResults) {
        if (result.triggered) {
            context.systemState.activeAlertCount++;
        }
    }
}

String StateManager::buildStateJson(const SystemContext& context) const {
    JsonDocument doc;
    doc["system"]["configName"] = context.systemState.configName;
    doc["system"]["configVersion"] = context.systemState.configVersion;
    doc["system"]["configLoaded"] = context.systemState.configLoaded;
    doc["system"]["degradedMode"] = context.systemState.degradedMode;
    doc["system"]["sensorCount"] = context.systemState.sensorCount;
    doc["system"]["activeAlertCount"] = context.systemState.activeAlertCount;

    JsonArray sensors = doc["sensors"].to<JsonArray>();
    for (const auto& value : context.sensorValues) {
        JsonObject o = sensors.add<JsonObject>();
        o["sensorId"] = value.sensorId;
        o["role"] = value.role;
        o["value"] = value.normalizedValue;
        o["valid"] = value.valid;
        o["quality"] = value.quality;
        o["timestampMs"] = value.timestampMs;
    }

    JsonArray outputs = doc["outputs"].to<JsonArray>();
    for (const auto& output : context.outputStates) {
        JsonObject o = outputs.add<JsonObject>();
        o["outputId"] = output.outputId;
        o["state"] = output.currentState;
        o["blocked"] = output.blocked;
        o["overridden"] = output.overridden;
        o["source"] = output.source;
    }

    JsonArray analyses = doc["analysisResults"].to<JsonArray>();
    for (const auto& result : context.analysisResults) {
        JsonObject o = analyses.add<JsonObject>();
        o["ruleId"] = result.ruleId;
        o["resultCode"] = result.resultCode;
        o["severity"] = result.severity;
        o["variable"] = result.variable;
        o["triggered"] = result.triggered;
        o["message"] = result.message;
    }

    JsonArray advice = doc["advice"].to<JsonArray>();
    for (const auto& item : context.adviceResults) {
        JsonObject o = advice.add<JsonObject>();
        o["resultCode"] = item.resultCode;
        o["preventive"] = item.preventive;
        o["corrective"] = item.corrective;
    }

    String output;
    serializeJson(doc, output);
    return output;
}

} // namespace Aqua
