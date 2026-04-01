#include "ManualTestManager.h"
#include <math.h>

namespace Aqua {

ManualTestManager::ManualTestManager(LogManager& logger) : m_logger(logger) {}

bool ManualTestManager::storeTest(const std::vector<ManualTestDefinition>& definitions,
                                  std::vector<ManualTestValue>& values,
                                  const String& testId,
                                  double value,
                                  String& outError) {
    for (const auto& def : definitions) {
        if (def.id != testId) {
            continue;
        }

        if (!isnan(def.minValue) && value < def.minValue) {
            outError = "Valeur inférieure au minimum autorisé";
            return false;
        }
        if (!isnan(def.maxValue) && value > def.maxValue) {
            outError = "Valeur supérieure au maximum autorisé";
            return false;
        }

        for (auto& existing : values) {
            if (existing.testId == testId) {
                existing.value = value;
                existing.timestampMs = millis();
                existing.expiresAtMs = existing.timestampMs + def.validityMs;
                existing.valid = true;
                m_logger.info("ManualTestManager", "Test manuel mis à jour: " + testId + "=" + String(value));
                return true;
            }
        }

        ManualTestValue test;
        test.testId = def.id;
        test.role = def.role;
        test.value = value;
        test.timestampMs = millis();
        test.expiresAtMs = test.timestampMs + def.validityMs;
        test.valid = true;
        values.push_back(test);

        m_logger.info("ManualTestManager", "Test manuel ajouté: " + testId + "=" + String(value));
        return true;
    }

    outError = "Test manuel inconnu: " + testId;
    return false;
}

void ManualTestManager::purgeExpired(std::vector<ManualTestValue>& values) {
    uint64_t now = millis();
    for (auto& test : values) {
        if (test.valid && test.expiresAtMs > 0 && now > test.expiresAtMs) {
            test.valid = false;
        }
    }
}

bool ManualTestManager::getValueByRole(const std::vector<ManualTestValue>& values, const String& role, double& outValue) const {
    for (const auto& value : values) {
        if (value.role == role && value.valid) {
            outValue = value.value;
            return true;
        }
    }
    return false;
}

} // namespace Aqua
