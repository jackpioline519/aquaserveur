#include "TimeService.h"
#include <time.h>

namespace Aqua {

uint64_t TimeService::nowMs() const {
    return millis();
}

String TimeService::currentDayCode() const {
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) {
        return "mon";
    }

    static const char* days[] = {"sun", "mon", "tue", "wed", "thu", "fri", "sat"};
    return days[timeinfo.tm_wday];
}

uint16_t TimeService::currentMinutesOfDay() const {
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) {
        return 0;
    }
    return static_cast<uint16_t>(timeinfo.tm_hour * 60 + timeinfo.tm_min);
}

bool TimeService::parseHourMinute(const String& hhmm, uint16_t& outMinutes) const {
    if (hhmm.length() != 5 || hhmm.charAt(2) != ':') {
        return false;
    }

    int hours = hhmm.substring(0, 2).toInt();
    int minutes = hhmm.substring(3, 5).toInt();
    if (hours < 0 || hours > 23 || minutes < 0 || minutes > 59) {
        return false;
    }

    outMinutes = static_cast<uint16_t>(hours * 60 + minutes);
    return true;
}

bool TimeService::isDayAllowed(const std::vector<String>& allowedDays, const String& currentDay) const {
    if (allowedDays.empty()) {
        return true;
    }

    for (const auto& d : allowedDays) {
        if (d == currentDay) {
            return true;
        }
    }
    return false;
}

} // namespace Aqua
