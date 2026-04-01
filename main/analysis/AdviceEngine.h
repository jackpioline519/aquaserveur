#pragma once

#include "../models/ConfigModels.h"
#include "../models/AnalysisModels.h"
#include <vector>

namespace Aqua {

class AdviceEngine {
public:
    void build(const ServerConfig& config,
               const std::vector<AnalysisResult>& analysisResults,
               std::vector<AdviceResult>& adviceResults) const;
};

} // namespace Aqua
