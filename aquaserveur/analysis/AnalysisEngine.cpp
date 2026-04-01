#include "AnalysisEngine.h"

namespace Aqua {

AnalysisEngine::AnalysisEngine(LogManager& logger, const SensorManager& sensorManager, const ManualTestManager& manualTestManager)
    : m_logger(logger), m_sensorManager(sensorManager), m_manualTestManager(manualTestManager) {}

void AnalysisEngine::run(const ServerConfig& config,
                         const std::vector<RuntimeSensorValue>& sensorValues,
                         const std::vector<ManualTestValue>& manualTests,
                         std::vector<AnalysisResult>& results) {
    results.clear();

    for (const auto& rule : config.analysisRules) {
        if (!rule.enabled) {
            continue;
        }

        AnalysisResult result;
        result.ruleId = rule.id;
        result.resultCode = rule.resultCode;
        result.severity = rule.severity;
        result.variable = rule.variable;
        result.message = rule.message;

        double variableValue = NAN;
        if (!resolveVariable(rule.variable, sensorValues, manualTests, variableValue)) {
            result.triggered = false;
            result.message = "Variable indisponible: " + rule.variable;
            results.push_back(result);
            continue;
        }

        result.triggered = evaluateCondition(rule.condition, variableValue, rule.value);
        if (result.triggered && result.message.isEmpty()) {
            result.message = rule.variable + " -> " + String(variableValue) + " (seuil " + String(rule.value) + ")";
        }
        results.push_back(result);
    }
}

bool AnalysisEngine::resolveVariable(const String& variable,
                                     const std::vector<RuntimeSensorValue>& sensorValues,
                                     const std::vector<ManualTestValue>& manualTests,
                                     double& outValue) const {
    if (m_sensorManager.getValueByRole(sensorValues, variable, outValue)) {
        return true;
    }
    if (m_manualTestManager.getValueByRole(manualTests, variable, outValue)) {
        return true;
    }
    return false;
}

bool AnalysisEngine::evaluateCondition(const String& condition, double left, double right) const {
    if (condition == "greater_than") return left > right;
    if (condition == "greater_or_equal") return left >= right;
    if (condition == "lower_than") return left < right;
    if (condition == "lower_or_equal") return left <= right;
    if (condition == "equal") return left == right;
    if (condition == "not_equal") return left != right;
    return false;
}

} // namespace Aqua
