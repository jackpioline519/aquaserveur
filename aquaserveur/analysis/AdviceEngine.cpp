#include "AdviceEngine.h"

namespace Aqua {

void AdviceEngine::build(const ServerConfig& config,
                         const std::vector<AnalysisResult>& analysisResults,
                         std::vector<AdviceResult>& adviceResults) const {
    adviceResults.clear();

    for (const auto& result : analysisResults) {
        if (!result.triggered) {
            continue;
        }

        for (const auto& advice : config.advice) {
            if (advice.resultCode == result.resultCode) {
                AdviceResult item;
                item.resultCode = advice.resultCode;
                item.preventive = advice.preventive;
                item.corrective = advice.corrective;
                adviceResults.push_back(item);
                break;
            }
        }
    }
}

} // namespace Aqua
