#pragma once

#include "esp_err.h"

namespace Aqua {

    class SdCardManager
    {
    public:
        esp_err_t Mount();
        esp_err_t Unmount();

    private:
        void* m_card = nullptr;
        bool m_mounted = false;
    };

}