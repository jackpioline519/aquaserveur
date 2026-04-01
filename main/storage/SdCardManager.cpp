#include "storage/SdCardManager.h"

#include <cstdio>

#include "driver/gpio.h"
#include "driver/spi_common.h"
#include "esp_log.h"
#include "esp_vfs_fat.h"
#include "sdmmc_cmd.h"

namespace {
    constexpr const char* TAG = "SdCardManager";

    // A adapter selon ton câblage réel
    constexpr gpio_num_t PIN_NUM_MISO = GPIO_NUM_13;
    constexpr gpio_num_t PIN_NUM_MOSI = GPIO_NUM_11;
    constexpr gpio_num_t PIN_NUM_CLK = GPIO_NUM_12;
    constexpr gpio_num_t PIN_NUM_CS = GPIO_NUM_10;

    constexpr const char* MOUNT_POINT = "/sdcard";
}

namespace Aqua {

    esp_err_t SdCardManager::Mount()
    {
        if (m_mounted) {
            ESP_LOGW(TAG, "Carte SD deja montee");
            return ESP_OK;
        }

        sdmmc_host_t host = SDSPI_HOST_DEFAULT();
        spi_host_device_t spiHost = SPI2_HOST;
        host.slot = spiHost;

        spi_bus_config_t bus_cfg = {};
        bus_cfg.mosi_io_num = PIN_NUM_MOSI;
        bus_cfg.miso_io_num = PIN_NUM_MISO;
        bus_cfg.sclk_io_num = PIN_NUM_CLK;
        bus_cfg.quadwp_io_num = -1;
        bus_cfg.quadhd_io_num = -1;
        bus_cfg.data4_io_num = -1;
        bus_cfg.data5_io_num = -1;
        bus_cfg.data6_io_num = -1;
        bus_cfg.data7_io_num = -1;
        bus_cfg.max_transfer_sz = 4000;

        esp_err_t ret = spi_bus_initialize(spiHost, &bus_cfg, SDSPI_DEFAULT_DMA);
        if (ret != ESP_OK && ret != ESP_ERR_INVALID_STATE) {
            ESP_LOGE(TAG, "spi_bus_initialize failed: %s", esp_err_to_name(ret));
            return ret;
        }

        sdspi_device_config_t slot_config = SDSPI_DEVICE_CONFIG_DEFAULT();
        slot_config.host_id = spiHost;
        slot_config.gpio_cs = PIN_NUM_CS;
        slot_config.gpio_cd = SDSPI_SLOT_NO_CD;
        slot_config.gpio_wp = SDSPI_SLOT_NO_WP;
        slot_config.gpio_int = GPIO_NUM_NC;

        esp_vfs_fat_sdmmc_mount_config_t mount_config = {};
        mount_config.format_if_mount_failed = false;
        mount_config.max_files = 5;
        mount_config.allocation_unit_size = 16 * 1024;
        mount_config.disk_status_check_enable = false;
        mount_config.use_one_fat = false;

        sdmmc_card_t* card = nullptr;

        ret = esp_vfs_fat_sdspi_mount(MOUNT_POINT, &host, &slot_config, &mount_config, &card);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "Impossible de monter la carte SD: %s", esp_err_to_name(ret));
            return ret;
        }

        m_card = card;
        m_mounted = true;

        ESP_LOGI(TAG, "Carte SD montee sur %s", MOUNT_POINT);
        sdmmc_card_print_info(stdout, card);

        return ESP_OK;
    }

    esp_err_t SdCardManager::Unmount()
    {
        if (!m_mounted) {
            ESP_LOGW(TAG, "Carte SD non montee");
            return ESP_OK;
        }

        esp_vfs_fat_sdcard_unmount(MOUNT_POINT, static_cast<sdmmc_card_t*>(m_card));

        m_card = nullptr;
        m_mounted = false;

        ESP_LOGI(TAG, "Carte SD demontee");
        return ESP_OK;
    }

}