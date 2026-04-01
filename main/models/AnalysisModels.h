#pragma once

#include <string>
#include <vector>

namespace Aqua {

    struct AdviceItem {
        std::string resultCode;
        std::string preventive;
        std::string corrective;
    };

    struct AnalysisSnapshot {
        std::vector<std::string> alerts;
        std::vector<AdviceItem> advices;
    };

}