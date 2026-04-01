#include "SensorManager.h"
#include <math.h>

namespace Aqua {

SensorManager::SensorManager(LogManager& logger) : m_logger(logger) {}

void SensorManager::initialize(const std::vector<SensorDefinition>& sensors, std::vector<RuntimeSensorValue>& runtimeValues) {
    runtimeValues.clear();
    for (const auto& sensor : sensors) {
        pinMode(sensor.pin, INPUT);

        RuntimeSensorValue value;
        value.sensorId = sensor.id;
        value.role = sensor.role;
        value.quality = "init";
        runtimeValues.push_back(value);

        m_logger.info("SensorManager", "Capteur initialisé: " + sensor.id + " (pin=" + String(sensor.pin) + ")");
    }
}

void SensorManager::readSensors(const std::vector<SensorDefinition>& sensors, std::vector<RuntimeSensorValue>& runtimeValues) {
    uint64_t now = millis();

    for (size_t i = 0; i < sensors.size() && i < runtimeValues.size(); ++i) {
        const auto& def = sensors[i];
        auto& current = runtimeValues[i];

        if (!def.enabled) {
            current.valid = false;
            current.quality = "disabled";
            continue;
        }

        if (current.timestampMs > 0 && now - current.timestampMs < def.readIntervalMs) {
            continue;
        }

        double raw = readMockSensor(def);
        current.rawValue = raw;
        current.normalizedValue = raw;
        current.timestampMs = now;
        current.valid = !isnan(raw);
        current.quality = current.valid ? "ok" : "invalid";
    }
}

bool SensorManager::getValueByRole(const std::vector<RuntimeSensorValue>& runtimeValues, const String& role, double& outValue) const {
    for (const auto& value : runtimeValues) {
        if (value.role == role && value.valid) {
            outValue = value.normalizedValue;
            return true;
        }
    }
    return false;
}

double SensorManager::readMockSensor(const SensorDefinition& sensor) const {
    // Point d’extension matériel:
    // remplacer cette partie par le driver réel selon sensor.type et sensor.protocol.
    if (sensor.type == "temperature") {
        return 24.0 + sin(millis() / 10000.0) * 2.0;
    }
    if (sensor.type == "ph") {
        return 6.8 + sin(millis() / 12000.0) * 0.2;
    }
    if (sensor.type == "level") {
        return 80.0;
    }

    int value = analogRead(sensor.pin);
    return static_cast<double>(value);
}

} // namespace Aqua
