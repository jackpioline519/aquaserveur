#pragma once


#include <deque>

namespace Aqua {

struct LogEntry {
    uint64_t timestampMs = 0;
    String level;
    String source;
    String message;
};

class LogManager {
public:
    void begin(size_t maxEntries, const String& level);
    void info(const String& source, const String& message);
    void warn(const String& source, const String& message);
    void error(const String& source, const String& message);
    void debug(const String& source, const String& message);

    const std::deque<LogEntry>& entries() const { return m_entries; }

private:
    void push(const String& level, const String& source, const String& message);
    bool shouldLog(const String& level) const;

    std::deque<LogEntry> m_entries;
    size_t m_maxEntries = 1000;
    String m_level = "info";
};

} // namespace Aqua
