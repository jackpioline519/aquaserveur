#pragma once

#include "../support/LogManager.h"
#include <functional>

namespace Aqua {

class CommManager {
public:
    using MessageCallback = std::function<void(const String&)>;

    explicit CommManager(LogManager& logger);

    void begin(uint16_t port);
    void setOnMessageReceived(MessageCallback callback);
    void loop();
    void publish(const String& jsonMessage);

private:
    LogManager& m_logger;
    MessageCallback m_callback;
    uint16_t m_port = 8080;
};

} // namespace Aqua
