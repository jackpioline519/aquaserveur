#include "ConfigValidator.h"
#include <cmath>
#include <set>

namespace Aqua {

bool ConfigValidator::validate(const ServerConfig& config, std::vector<String>& errors, std::vector<String>& warnings) const {
    std::set<String> ids;
    std::set<String> variables;
    std::set<String> outputIds;
    std::set<String> resultCodes;

    if (config.system.name.isEmpty()) {
        errors.push_back("system.name est obligatoire");
    }

    for (const auto& sensor : config.sensors) {
        if (sensor.id.isEmpty()) errors.push_back("Un capteur sans id a été trouvé");
        if (sensor.role.isEmpty()) errors.push_back("Le capteur " + sensor.id + " n'a pas de role");
        if (sensor.pin < 0) errors.push_back("Le capteur " + sensor.id + " a un pin invalide");
        if (!ids.insert(sensor.id).second) errors.push_back("Id dupliqué: " + sensor.id);
        variables.insert(sensor.role);
    }

    for (const auto& output : config.outputs) {
        if (output.id.isEmpty()) errors.push_back("Une sortie sans id a été trouvée");
        if (output.pin < 0) errors.push_back("La sortie " + output.id + " a un pin invalide");
        if (!ids.insert(output.id).second) errors.push_back("Id dupliqué: " + output.id);
        outputIds.insert(output.id);
    }

    for (const auto& test : config.manualTests) {
        if (test.id.isEmpty()) errors.push_back("Un test manuel sans id a été trouvé");
        if (test.role.isEmpty()) errors.push_back("Le test manuel " + test.id + " n'a pas de role");
        if (!ids.insert(test.id).second) errors.push_back("Id dupliqué: " + test.id);
        variables.insert(test.role);
        if (!isnan(test.minValue) && !isnan(test.maxValue) && test.minValue > test.maxValue) {
            errors.push_back("Bornes invalides pour le test manuel " + test.id);
        }
    }

    for (const auto& rule : config.analysisRules) {
        if (rule.id.isEmpty()) errors.push_back("Une règle sans id a été trouvée");
        if (rule.variable.isEmpty()) errors.push_back("La règle " + rule.id + " n'a pas de variable");
        if (rule.resultCode.isEmpty()) errors.push_back("La règle " + rule.id + " n'a pas de resultCode");
        if (!variables.count(rule.variable)) {
            warnings.push_back("La règle " + rule.id + " référence une variable inconnue: " + rule.variable);
        }
        resultCodes.insert(rule.resultCode);
    }

    for (const auto& advice : config.advice) {
        if (advice.resultCode.isEmpty()) {
            errors.push_back("Un conseil sans resultCode a été trouvé");
        } else if (!resultCodes.count(advice.resultCode)) {
            warnings.push_back("Conseil orphelin pour resultCode=" + advice.resultCode);
        }
    }

    for (const auto& routine : config.routines) {
        if (routine.id.isEmpty()) errors.push_back("Une routine sans id a été trouvée");
        if (!outputIds.count(routine.outputId)) {
            warnings.push_back("Routine " + routine.id + " liée à une sortie inconnue: " + routine.outputId);
        }
        if (routine.start.length() != 5 || routine.end.length() != 5) {
            errors.push_back("Routine " + routine.id + " avec heure invalide");
        }
    }

    if (config.outputs.empty()) {
        warnings.push_back("Aucune sortie déclarée");
    }
    if (config.sensors.empty() && config.manualTests.empty()) {
        warnings.push_back("Aucune source de donnée déclarée");
    }

    return errors.empty();
}

} // namespace Aqua
