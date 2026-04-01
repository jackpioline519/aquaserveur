#include "ConfigLoader.h"
#include <ArduinoJson.h>
#include <FS.h>
#include <LittleFS.h>

namespace Aqua {

static void loadSensor(const JsonObjectConst& obj, SensorDefinition& s) {
    s.id = obj["id"] | "";
    s.type = obj["type"] | "";
    s.protocol = obj["protocol"] | "";
    s.pin = obj["pin"] | -1;
    s.unit = obj["unit"] | "";
    s.role = obj["role"] | "";
    s.readIntervalMs = obj["readIntervalMs"] | 5000;
    s.enabled = obj["enabled"] | true;
}

static void loadOutput(const JsonObjectConst& obj, OutputDefinition& o) {
    o.id = obj["id"] | "";
    o.pin = obj["pin"] | -1;
    o.role = obj["role"] | "";
    o.label = obj["label"] | "";
    o.defaultState = obj["defaultState"] | "off";
    o.manualControlAllowed = obj["manualControlAllowed"] | false;
    o.enabled = obj["enabled"] | true;
}

static void loadManualTest(const JsonObjectConst& obj, ManualTestDefinition& t) {
    t.id = obj["id"] | "";
    t.label = obj["label"] | "";
    t.unit = obj["unit"] | "";
    t.role = obj["role"] | "";
    t.minValue = obj["minValue"].isNull() ? NAN : static_cast<double>(obj["minValue"].as<double>());
    t.maxValue = obj["maxValue"].isNull() ? NAN : static_cast<double>(obj["maxValue"].as<double>());
    t.validityMs = obj["validityMs"] | 86400000UL;
}

static void loadRule(const JsonObjectConst& obj, AnalysisRuleDefinition& r) {
    r.id = obj["id"] | "";
    r.variable = obj["variable"] | "";
    r.condition = obj["condition"] | "";
    r.value = obj["value"] | 0.0;
    r.severity = obj["severity"] | "info";
    r.resultCode = obj["resultCode"] | "";
    r.message = obj["message"] | "";
    r.enabled = obj["enabled"] | true;
}

static void loadAdvice(const JsonObjectConst& obj, AdviceDefinition& a) {
    a.resultCode = obj["resultCode"] | "";
    a.preventive = obj["preventive"] | "";
    a.corrective = obj["corrective"] | "";
}

static void loadRoutine(const JsonObjectConst& obj, RoutineDefinition& r) {
    r.id = obj["id"] | "";
    r.outputId = obj["outputId"] | "";
    r.start = obj["start"] | "00:00";
    r.end = obj["end"] | "00:00";
    r.priority = obj["priority"] | 0;
    r.enabled = obj["enabled"] | true;

    r.days.clear();
    JsonArrayConst days = obj["days"].as<JsonArrayConst>();
    for (JsonVariantConst d : days) {
        r.days.push_back(d.as<String>());
    }
}

bool ConfigLoader::loadFromFile(const String& path, ServerConfig& outConfig, String& outError) const {
    if (!LittleFS.begin(true)) {
        outError = "Impossible de monter LittleFS";
        return false;
    }

    File file = LittleFS.open(path, "r");
    if (!file) {
        outError = "Fichier JSON introuvable: " + path;
        return false;
    }

    String json;
    while (file.available()) {
        json += static_cast<char>(file.read());
    }
    file.close();

    return loadFromJsonString(json, outConfig, outError);
}

bool ConfigLoader::loadFromJsonString(const String& json, ServerConfig& outConfig, String& outError) const {
    JsonDocument doc;
    DeserializationError err = deserializeJson(doc, json);
    if (err) {
        outError = String("Erreur de parsing JSON: ") + err.c_str();
        return false;
    }

    outConfig = ServerConfig{};

    JsonObjectConst system = doc["system"].as<JsonObjectConst>();
    outConfig.system.name = system["name"] | "Aquarium";
    outConfig.system.configVersion = system["configVersion"] | "1.0";

    for (JsonObjectConst obj : doc["sensors"].as<JsonArrayConst>()) {
        SensorDefinition s;
        loadSensor(obj, s);
        outConfig.sensors.push_back(s);
    }

    for (JsonObjectConst obj : doc["outputs"].as<JsonArrayConst>()) {
        OutputDefinition o;
        loadOutput(obj, o);
        outConfig.outputs.push_back(o);
    }

    for (JsonObjectConst obj : doc["manualTests"].as<JsonArrayConst>()) {
        ManualTestDefinition t;
        loadManualTest(obj, t);
        outConfig.manualTests.push_back(t);
    }

    for (JsonObjectConst obj : doc["analysisRules"].as<JsonArrayConst>()) {
        AnalysisRuleDefinition r;
        loadRule(obj, r);
        outConfig.analysisRules.push_back(r);
    }

    for (JsonObjectConst obj : doc["advice"].as<JsonArrayConst>()) {
        AdviceDefinition a;
        loadAdvice(obj, a);
        outConfig.advice.push_back(a);
    }

    for (JsonObjectConst obj : doc["routines"].as<JsonArrayConst>()) {
        RoutineDefinition r;
        loadRoutine(obj, r);
        outConfig.routines.push_back(r);
    }

    JsonObjectConst ihm = doc["ihm"].as<JsonObjectConst>();
    outConfig.ihm.protocol = ihm["protocol"] | "websocket";
    outConfig.ihm.port = ihm["port"] | 8080;
    outConfig.ihm.publishIntervalMs = ihm["publishIntervalMs"] | 1000;

    JsonObjectConst logging = doc["logging"].as<JsonObjectConst>();
    outConfig.logging.level = logging["level"] | "info";
    outConfig.logging.maxEntries = logging["maxEntries"] | 1000;

    return true;
}

} // namespace Aqua
