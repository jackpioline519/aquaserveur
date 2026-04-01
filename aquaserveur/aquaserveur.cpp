#include <Arduino.h>

#include "core/BootManager.h"
#include "core/StateManager.h"
#include "core/SystemContext.h"
#include "config/ConfigLoader.h"
#include "config/ConfigValidator.h"
#include "config/ConfigManager.h"
#include "comm/CommManager.h"
#include "comm/CommandManager.h"
#include "sensors/SensorManager.h"
#include "outputs/OutputManager.h"
#include "analysis/AnalysisEngine.h"
#include "analysis/AdviceEngine.h"
#include "analysis/RoutineManager.h"
#include "analysis/ManualTestManager.h"
#include "support/LogManager.h"
#include "support/TimeService.h"

using namespace Aqua;

namespace {
    constexpr const char* kConfigPath = "/config.json";

    LogManager g_logger;
    ConfigLoader g_configLoader;
    ConfigValidator g_configValidator;
    SensorManager g_sensorManager(g_logger);
    OutputManager g_outputManager(g_logger);
    ManualTestManager g_manualTestManager(g_logger);
    ConfigManager g_configManager(g_configLoader, g_configValidator, g_logger);
    TimeService g_timeService;
    BootManager g_bootManager(g_configManager, g_sensorManager, g_outputManager, g_logger);
    AnalysisEngine g_analysisEngine(g_logger, g_sensorManager, g_manualTestManager);
    AdviceEngine g_adviceEngine;
    RoutineManager g_routineManager(g_logger, g_timeService, g_outputManager);
    StateManager g_stateManager;
    CommManager g_commManager(g_logger);
    SystemContext g_context;
    CommandManager g_commandManager(g_logger, g_outputManager, g_manualTestManager, g_configManager);
}

static void setupCommunication() {
    g_commManager.begin(g_context.activeConfig.ihm.port);

    g_commManager.setOnMessageReceived([](const String& rawMessage) {
        IhmCommand command;
        String error;
        if (!g_commandManager.decode(rawMessage, command, error)) {
            g_logger.warn("CommManager", error);
            return;
        }

        CommandResult result = g_commandManager.process(command, g_context);
        if (!result.success) {
            g_logger.warn("CommandManager", result.message);
        }
    });
}

static void publishStateIfNeeded() {
    uint64_t now = millis();
    if (now - g_context.systemState.lastPublishMs < g_context.activeConfig.ihm.publishIntervalMs) {
        return;
    }

    g_stateManager.compute(g_context);
    String json = g_stateManager.buildStateJson(g_context);
    g_commManager.publish(json);
    g_context.systemState.lastPublishMs = now;
}

void setup() {
    Serial.begin(115200);
    delay(500);

    g_logger.begin(1000, "debug");
    g_logger.info("Main", "Démarrage du serveur aquarium");

    String error;
    if (!g_bootManager.boot(kConfigPath, g_context, error)) {
        g_logger.error("Main", "Echec du boot: " + error);
    } else {
        g_logger.begin(g_context.activeConfig.logging.maxEntries, g_context.activeConfig.logging.level);
        g_logger.info("Main", "Boot terminé avec succès");
        setupCommunication();
    }
}

void loop() {
    if (!g_context.systemState.configLoaded) {
        delay(1000);
        return;
    }

    bool configChanged = false;
    if (g_commandManager.consumePendingConfig(g_context, configChanged) && configChanged) {
        String error;
        if (!g_bootManager.rebuildFromActiveConfig(g_context, error)) {
            g_logger.error("Main", "Rebuild impossible après changement de config: " + error);
        } else {
            setupCommunication();
        }
    }

    g_commManager.loop();
    g_outputManager.releaseExpiredOverrides(g_context.outputStates);
    g_manualTestManager.purgeExpired(g_context.manualTests);
    g_sensorManager.readSensors(g_context.activeConfig.sensors, g_context.sensorValues);
    g_routineManager.apply(g_context.activeConfig, g_context.outputStates);
    g_analysisEngine.run(g_context.activeConfig, g_context.sensorValues, g_context.manualTests, g_context.analysisResults);
    g_adviceEngine.build(g_context.activeConfig, g_context.analysisResults, g_context.adviceResults);
    publishStateIfNeeded();

    delay(50);
}
