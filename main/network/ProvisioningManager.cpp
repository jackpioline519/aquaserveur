#include "network/ProvisioningManager.h"

#include <cstdio>
#include <cstring>
#include <string>

#include "storage/SettingsStore.h"

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"

#include "esp_event.h"
#include "esp_log.h"
#include "esp_mac.h"
#include "esp_netif.h"
#include "esp_wifi.h"
#include "nvs_flash.h"

#include "network_provisioning/manager.h"
#include "network_provisioning/scheme_ble.h"

namespace {
    constexpr const char* TAG = "ProvisioningManager";

    constexpr EventBits_t WIFI_CONNECTED_BIT = BIT0;
    constexpr EventBits_t PROV_SUCCESS_BIT = BIT1;
    constexpr EventBits_t PROV_FAILED_BIT = BIT2;

    EventGroupHandle_t g_eventGroup = nullptr;
    Aqua::SettingsStore g_settingsStore;
}

namespace Aqua {

    ProvisioningManager::ProvisioningManager()
    {
    }

    esp_err_t ProvisioningManager::InitBaseNetwork()
    {
        ESP_ERROR_CHECK(esp_netif_init());

        esp_err_t err = esp_event_loop_create_default();
        if (err != ESP_OK && err != ESP_ERR_INVALID_STATE) {
            return err;
        }

        if (g_eventGroup == nullptr) {
            g_eventGroup = xEventGroupCreate();
            if (g_eventGroup == nullptr) {
                return ESP_ERR_NO_MEM;
            }
        }

        esp_netif_create_default_wifi_sta();

        wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
        ESP_ERROR_CHECK(esp_wifi_init(&cfg));

        ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_FLASH));

        ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT,
            ESP_EVENT_ANY_ID,
            &ProvisioningManager::EventHandler,
            nullptr));

        ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT,
            IP_EVENT_STA_GOT_IP,
            &ProvisioningManager::EventHandler,
            nullptr));

        ESP_ERROR_CHECK(esp_event_handler_register(NETWORK_PROV_EVENT,
            ESP_EVENT_ANY_ID,
            &ProvisioningManager::EventHandler,
            nullptr));

        return ESP_OK;
    }

    void ProvisioningManager::EventHandler(void*,
        esp_event_base_t event_base,
        int32_t event_id,
        void* event_data)
    {
        if (event_base == WIFI_EVENT) {
            if (event_id == WIFI_EVENT_STA_START) {
                esp_wifi_connect();
            }
            else if (event_id == WIFI_EVENT_STA_DISCONNECTED) {
                ESP_LOGW(TAG, "Wi-Fi deconnecte, nouvelle tentative...");
                esp_wifi_connect();
            }
        }
        else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
            ESP_LOGI(TAG, "Wi-Fi connecte, IP obtenue");
            xEventGroupSetBits(g_eventGroup, WIFI_CONNECTED_BIT);
        }
        else if (event_base == NETWORK_PROV_EVENT) {
            switch (event_id) {
            case NETWORK_PROV_START:
                ESP_LOGI(TAG, "Provisioning BLE demarre");
                break;

            case NETWORK_PROV_WIFI_CRED_RECV:
                ESP_LOGI(TAG, "Identifiants Wi-Fi recus");
                break;

            case NETWORK_PROV_WIFI_CRED_SUCCESS:
                ESP_LOGI(TAG, "Provisioning Wi-Fi reussi");
                xEventGroupSetBits(g_eventGroup, PROV_SUCCESS_BIT);
                break;

            case NETWORK_PROV_WIFI_CRED_FAIL:
                ESP_LOGE(TAG, "Provisioning Wi-Fi echoue");
                xEventGroupSetBits(g_eventGroup, PROV_FAILED_BIT);
                break;

            case NETWORK_PROV_END:
                ESP_LOGI(TAG, "Provisioning termine");
                network_prov_mgr_deinit();
                break;

            default:
                break;
            }
        }

        (void)event_data;
    }

    std::string ProvisioningManager::BuildDefaultServiceName() const
    {
        uint8_t mac[6] = {};
        esp_read_mac(mac, ESP_MAC_WIFI_STA);

        char buf[32];
        std::snprintf(buf, sizeof(buf), "Aqua-%02X%02X%02X", mac[3], mac[4], mac[5]);
        return std::string(buf);
    }

    std::string ProvisioningManager::BuildDefaultDeviceName() const
    {
        return BuildDefaultServiceName();
    }

    esp_err_t ProvisioningManager::DeviceNameEndpointHandler(uint32_t,
        const uint8_t* inbuf,
        ssize_t inlen,
        uint8_t** outbuf,
        ssize_t* outlen,
        void*)
    {
        if (inbuf == nullptr || inlen <= 0 || outbuf == nullptr || outlen == nullptr) {
            return ESP_ERR_INVALID_ARG;
        }

        std::string receivedName(reinterpret_cast<const char*>(inbuf),
            static_cast<size_t>(inlen));

        while (!receivedName.empty() &&
            (receivedName.back() == '\0' ||
                receivedName.back() == '\n' ||
                receivedName.back() == '\r' ||
                receivedName.back() == ' ')) {
            receivedName.pop_back();
        }

        const char* msg = nullptr;

        if (receivedName.empty()) {
            msg = "device_name_empty";
        }
        else {
            esp_err_t err = g_settingsStore.SaveDeviceName(receivedName);
            msg = (err == ESP_OK) ? "ok" : "save_failed";
        }

        *outlen = std::strlen(msg);
        *outbuf = static_cast<uint8_t*>(malloc(*outlen));
        if (*outbuf == nullptr) {
            *outlen = 0;
            return ESP_ERR_NO_MEM;
        }

        std::memcpy(*outbuf, msg, *outlen);
        return ESP_OK;
    }

    esp_err_t ProvisioningManager::StartProvisioningService()
    {
        network_prov_mgr_config_t config = {};
        config.scheme = network_prov_scheme_ble;
        config.scheme_event_handler = NETWORK_PROV_SCHEME_BLE_EVENT_HANDLER_FREE_BTDM;

        ESP_ERROR_CHECK(network_prov_mgr_init(config));

        ESP_ERROR_CHECK(network_prov_mgr_endpoint_create("device-name"));

        std::string serviceName = BuildDefaultServiceName();

        network_prov_security_t security = NETWORK_PROV_SECURITY_1;
        const char* pop = "Aqua1234";

        ESP_ERROR_CHECK(network_prov_mgr_start_wifi_provisioning(
            security,
            pop,
            serviceName.c_str(),
            nullptr));

        ESP_ERROR_CHECK(network_prov_mgr_endpoint_register(
            "device-name",
            &ProvisioningManager::DeviceNameEndpointHandler,
            nullptr));

        ESP_LOGI(TAG, "BLE provisioning actif. Service: %s", serviceName.c_str());
        ESP_LOGI(TAG, "PoP: %s", pop);

        return ESP_OK;
    }

    esp_err_t ProvisioningManager::ConnectWifiWithSavedCredentials()
    {
        ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
        ESP_ERROR_CHECK(esp_wifi_start());

        EventBits_t bits = xEventGroupWaitBits(
            g_eventGroup,
            WIFI_CONNECTED_BIT,
            pdTRUE,
            pdFALSE,
            pdMS_TO_TICKS(30000)
        );

        if (bits & WIFI_CONNECTED_BIT) {
            return ESP_OK;
        }

        return ESP_FAIL;
    }

    esp_err_t ProvisioningManager::EnsureProvisionedAndConnected(std::string& deviceName)
    {
        bool provisioned = false;
        ESP_ERROR_CHECK(network_prov_mgr_is_wifi_provisioned(&provisioned));

        if (!provisioned) {
            ESP_LOGI(TAG, "Aucun Wi-Fi sauvegarde, lancement du provisioning BLE");

            std::string defaultName;
            if (g_settingsStore.LoadDeviceName(defaultName) != ESP_OK) {
                defaultName = BuildDefaultDeviceName();
                g_settingsStore.SaveDeviceName(defaultName);
            }

            ESP_ERROR_CHECK(StartProvisioningService());

            EventBits_t bits = xEventGroupWaitBits(
                g_eventGroup,
                WIFI_CONNECTED_BIT | PROV_FAILED_BIT,
                pdTRUE,
                pdFALSE,
                portMAX_DELAY
            );

            if (bits & PROV_FAILED_BIT) {
                return ESP_FAIL;
            }
        }
        else {
            ESP_LOGI(TAG, "Wi-Fi deja provisionne, connexion avec les identifiants enregistres");
            ESP_ERROR_CHECK(ConnectWifiWithSavedCredentials());
        }

        if (g_settingsStore.LoadDeviceName(deviceName) != ESP_OK) {
            deviceName = BuildDefaultDeviceName();
            g_settingsStore.SaveDeviceName(deviceName);
        }

        return ESP_OK;
    }

}