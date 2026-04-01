#include "storage/SettingsStore.h"
#include "nvs.h"
#include <vector>

namespace Aqua {

    esp_err_t SettingsStore::SaveDeviceName(const std::string& name)
    {
        nvs_handle_t handle;
        esp_err_t err = nvs_open(kNamespace, NVS_READWRITE, &handle);
        if (err != ESP_OK) {
            return err;
        }

        err = nvs_set_str(handle, kKeyDeviceName, name.c_str());
        if (err == ESP_OK) {
            err = nvs_commit(handle);
        }

        nvs_close(handle);
        return err;
    }

    esp_err_t SettingsStore::LoadDeviceName(std::string& name)
    {
        nvs_handle_t handle;
        esp_err_t err = nvs_open(kNamespace, NVS_READONLY, &handle);
        if (err != ESP_OK) {
            return err;
        }

        size_t requiredSize = 0;
        err = nvs_get_str(handle, kKeyDeviceName, nullptr, &requiredSize);
        if (err != ESP_OK) {
            nvs_close(handle);
            return err;
        }

        std::vector<char> buffer(requiredSize);
        err = nvs_get_str(handle, kKeyDeviceName, buffer.data(), &requiredSize);
        nvs_close(handle);

        if (err != ESP_OK) {
            return err;
        }

        name = std::string(buffer.data());
        return ESP_OK;
    }

    esp_err_t SettingsStore::ClearAll()
    {
        nvs_handle_t handle;
        esp_err_t err = nvs_open(kNamespace, NVS_READWRITE, &handle);
        if (err != ESP_OK) {
            return err;
        }

        err = nvs_erase_all(handle);
        if (err == ESP_OK) {
            err = nvs_commit(handle);
        }

        nvs_close(handle);
        return err;
    }

}