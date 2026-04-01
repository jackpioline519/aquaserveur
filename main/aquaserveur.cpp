#include <cstdio>
#include <string>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_err.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "network/ProvisioningManager.h"
#include "storage/SdCardManager.h"

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
    constexpr const char* TAG = "Rhéanova";
    constexpr const char* kConfigPath = "/sdcard/config.json";

    LogManager g_logger;
    ConfigLoader g_configLoader;
    ConfigValidator g_configValidator;
    TimeService g_timeService;
}

static esp_err_t InitNvs()
{
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    return err;
}

static void RunServer(const std::string& deviceName)
{
    ESP_LOGI(TAG, "Démarrage du système aquarium [%s]", deviceName.c_str());

    SystemContext context;
    context.logger = &g_logger;
    context.timeService = &g_timeService;

    ConfigManager configManager(&g_configLoader, &g_configValidator, &g_logger);
    context.configManager = &configManager;

    StateManager stateManager;
    context.stateManager = &stateManager;

    SensorManager sensorManager(g_logger);
    context.sensorManager = &sensorManager;

    OutputManager outputManager(g_logger);
    context.outputManager = &outputManager;

    AnalysisEngine analysisEngine(g_logger);
    context.analysisEngine = &analysisEngine;

    AdviceEngine adviceEngine(g_logger);
    context.adviceEngine = &adviceEngine;

    RoutineManager routineManager(g_logger);
    context.routineManager = &routineManager;

    ManualTestManager manualTestManager(g_logger);
    context.manualTestManager = &manualTestManager;

    CommManager commManager(g_logger);
    context.commManager = &commManager;

    CommandManager commandManager(g_logger);
    context.commandManager = &commandManager;

    BootManager bootManager(g_logger);

    if (!configManager.LoadFromFile(kConfigPath)) {
        ESP_LOGE(TAG, "Impossible de charger le fichier de configuration: %s", kConfigPath);
        return;
    }

    if (!bootManager.Initialize(context)) {
        ESP_LOGE(TAG, "Échec initialisation BootManager");
        return;
    }

    ESP_LOGI(TAG, "Serveur aquarium démarré");

    while (true) {
        routineManager.Update(context);
        sensorManager.Update(context);
        analysisEngine.Update(context);
        commManager.Update(context);

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

extern "C" void app_main(void)
{
    ESP_ERROR_CHECK(InitNvs());

    ProvisioningManager provisioningManager;
    ESP_ERROR_CHECK(provisioningManager.InitBaseNetwork());

    std::string deviceName;
    ESP_ERROR_CHECK(provisioningManager.EnsureProvisionedAndConnected(deviceName));

    SdCardManager sdCardManager;
    ESP_ERROR_CHECK(sdCardManager.Mount());

    RunServer(deviceName);
}