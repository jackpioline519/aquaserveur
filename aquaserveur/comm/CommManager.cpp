#include "CommManager.h"

namespace Aqua {

CommManager::CommManager(LogManager& logger) : m_logger(logger) {}

void CommManager::begin(uint16_t port) {
    m_port = port;
    m_logger.info("CommManager", "Canal IHM prêt sur le port " + String(m_port) + " (stub à brancher sur WebSocket/HTTP)");
}

void CommManager::setOnMessageReceived(MessageCallback callback) {
    m_callback = callback;
}

void CommManager::loop() {
    // Point d’extension réel:
    // ici brancher AsyncWebServer, WebSocket, MQTT ou autre protocole IHM.
}

void CommManager::publish(const String& jsonMessage) {
    m_logger.debug("CommManager", "Publication IHM: " + jsonMessage);
    Serial.println(jsonMessage);
}

} // namespace Aqua
