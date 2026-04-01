#pragma once

#include <Arduino.h>
#include <vector>

namespace Aqua {

class TimeService {
public:
    uint64_t nowMs() const;
    String currentDayCode() const;
    uint16_t currentMinutesOfDay() const;
    bool parseHourMinute(const String& hhmm, uint16_t& outMinutes) const;
    bool isDayAllowed(const std::vector<String>& allowedDays, const String& currentDay) const;
};

} // namespace Aqua
