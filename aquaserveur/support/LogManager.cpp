#include "LogManager.h"
#include <Arduino.h>

namespace Aqua {

void LogManager::begin(size_t maxEntries, const String& level) {
    m_maxEntries = maxEntries > 0 ? maxEntries : 1000;
    m_level = level;
    m_entries.clear();
}

void LogManager::info(const String& source, const String& message) { push("info", source, message); }
void LogManager::warn(const String& source, const String& message) { push("warning", source, message); }
void LogManager::error(const String& source, const String& message) { push("error", source, message); }
void LogManager::debug(const String& source, const String& message) { push("debug", source, message); }

bool LogManager::shouldLog(const String& level) const {
    auto rank = [](const String& v) -> int {
        if (v == "debug") return 0;
        if (v == "info") return 1;
        if (v == "warning") return 2;
        if (v == "error") return 3;
        return 1;
    };
    return rank(level) >= rank(m_level);
}

void LogManager::push(const String& level, const String& source, const String& message) {
    if (!shouldLog(level)) {
        return;
    }

    LogEntry entry;
    entry.timestampMs = millis();
    entry.level = level;
    entry.source = source;
    entry.message = message;

    if (m_entries.size() >= m_maxEntries) {
        m_entries.pop_front();
    }
    m_entries.push_back(entry);

    Serial.printf("[%s] [%s] %s\n", level.c_str(), source.c_str(), message.c_str());
}

} // namespace Aqua
