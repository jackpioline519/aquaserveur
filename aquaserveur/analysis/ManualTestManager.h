#pragma once

#include "../models/ConfigModels.h"
#include "../models/RuntimeModels.h"
#include "../support/LogManager.h"
#include <vector>

namespace Aqua {

class ManualTestManager {
public:
    explicit ManualTestManager(LogManager& logger);

    bool storeTest(const std::vector<ManualTestDefinition>& definitions,
                   std::vector<ManualTestValue>& values,
                   const String& testId,
                   double value,
                   String& outError);
    void purgeExpired(std::vector<ManualTestValue>& values);
    bool getValueByRole(const std::vector<ManualTestValue>& values, const String& role, double& outValue) const;

private:
    LogManager& m_logger;
};

} // namespace Aqua
