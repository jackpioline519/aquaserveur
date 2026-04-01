#include "OutputManager.h"

namespace Aqua {

OutputManager::OutputManager(LogManager& logger) : m_logger(logger) {}

void OutputManager::initialize(const std::vector<OutputDefinition>& outputs, std::vector<OutputState>& states) {
    states.clear();

    for (const auto& output : outputs) {
        pinMode(output.pin, OUTPUT);
        bool initialState = output.defaultState == "on";
        digitalWrite(output.pin, initialState ? HIGH : LOW);

        OutputState state;
        state.outputId = output.id;
        state.currentState = initialState;
        state.source = "default";
        states.push_back(state);

        m_logger.info("OutputManager", "Sortie initialisée: " + output.id + " = " + String(initialState ? "ON" : "OFF"));
    }
}

bool OutputManager::applyOutputCommand(const std::vector<OutputDefinition>& outputs,
                                       std::vector<OutputState>& states,
                                       const String& outputId,
                                       bool newState,
                                       bool isManualOverride,
                                       uint32_t overrideDurationMs,
                                       String& outError) {
    for (size_t i = 0; i < outputs.size() && i < states.size(); ++i) {
        const auto& output = outputs[i];
        auto& state = states[i];

        if (output.id != outputId) {
            continue;
        }

        if (state.blocked) {
            outError = "La sortie est bloquée: " + outputId;
            return false;
        }

        if (isManualOverride && !output.manualControlAllowed) {
            outError = "Commande manuelle interdite pour la sortie: " + outputId;
            return false;
        }

        digitalWrite(output.pin, newState ? HIGH : LOW);
        state.currentState = newState;
        state.source = isManualOverride ? "manual" : "automatic";
        state.overridden = isManualOverride;
        state.overrideUntilMs = isManualOverride ? millis() + overrideDurationMs : 0;

        m_logger.info("OutputManager", "Sortie " + outputId + " -> " + String(newState ? "ON" : "OFF"));
        return true;
    }

    outError = "Sortie inconnue: " + outputId;
    return false;
}

bool OutputManager::findState(const std::vector<OutputState>& states, const String& outputId, OutputState*& outState) {
    for (auto& state : const_cast<std::vector<OutputState>&>(states)) {
        if (state.outputId == outputId) {
            outState = &state;
            return true;
        }
    }
    outState = nullptr;
    return false;
}

void OutputManager::releaseExpiredOverrides(std::vector<OutputState>& states) {
    uint64_t now = millis();
    for (auto& state : states) {
        if (state.overridden && state.overrideUntilMs > 0 && now >= state.overrideUntilMs) {
            state.overridden = false;
            state.overrideUntilMs = 0;
            state.source = "automatic";
            m_logger.info("OutputManager", "Fin d'override manuel pour " + state.outputId);
        }
    }
}

} // namespace Aqua
