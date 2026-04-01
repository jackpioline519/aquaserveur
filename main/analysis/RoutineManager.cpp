#include "RoutineManager.h"

namespace Aqua {

RoutineManager::RoutineManager(LogManager& logger, TimeService& timeService, OutputManager& outputManager)
    : m_logger(logger), m_timeService(timeService), m_outputManager(outputManager) {}

void RoutineManager::apply(const ServerConfig& config, std::vector<OutputState>& states) {
    const String currentDay = m_timeService.currentDayCode();
    const uint16_t nowMinutes = m_timeService.currentMinutesOfDay();

    for (const auto& routine : config.routines) {
        if (!routine.enabled) {
            continue;
        }

        if (!m_timeService.isDayAllowed(routine.days, currentDay)) {
            continue;
        }

        uint16_t startMinutes = 0;
        uint16_t endMinutes = 0;
        if (!m_timeService.parseHourMinute(routine.start, startMinutes) ||
            !m_timeService.parseHourMinute(routine.end, endMinutes)) {
            continue;
        }

        bool shouldBeOn = nowMinutes >= startMinutes && nowMinutes < endMinutes;

        OutputState* state = nullptr;
        if (!m_outputManager.findState(states, routine.outputId, state) || state == nullptr) {
            continue;
        }

        if (state->overridden) {
            continue;
        }

        String error;
        if (!m_outputManager.applyOutputCommand(config.outputs, states, routine.outputId, shouldBeOn, false, 0, error)) {
            m_logger.warn("RoutineManager", error);
        }
    }
}

} // namespace Aqua
