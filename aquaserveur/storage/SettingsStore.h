#pragma once

#include <string>
#include "esp_err.h"

namespace Aqua {

    struct DeviceSettings
    {
        std::string deviceName;
    };

    class SettingsStore
    {
    public:
        esp_err_t SaveDeviceName(const std::string& name);
        esp_err_t LoadDeviceName(std::string& name);
        esp_err_t ClearAll();

    private:
        static constexpr const char* kNamespace = "aquarium";
        static constexpr const char* kKeyDeviceName = "device_name";
    };

}