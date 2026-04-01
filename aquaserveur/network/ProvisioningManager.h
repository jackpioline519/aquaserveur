#pragma once

#include <string>
#include "esp_err.h"

namespace Aqua {

    class ProvisioningManager
    {
    public:
        ProvisioningManager();

        esp_err_t InitBaseNetwork();
        esp_err_t EnsureProvisionedAndConnected(std::string& deviceName);

    private:
        static void EventHandler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data);
        static esp_err_t DeviceNameEndpointHandler(uint32_t session_id,
            const uint8_t* inbuf, ssize_t inlen,
            uint8_t** outbuf, ssize_t* outlen,
            void* priv_data);

        std::string BuildDefaultServiceName() const;
        std::string BuildDefaultDeviceName() const;
        esp_err_t StartProvisioningService();
        esp_err_t ConnectWifiWithSavedCredentials();
    };

}