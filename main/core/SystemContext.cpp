#include "SystemContext.h"

namespace Aqua {

void SystemContext::clear() {
    activeConfig = ServerConfig{};
    sensorValues.clear();
    manualTests.clear();
    outputStates.clear();
    analysisResults.clear();
    adviceResults.clear();
    systemState = SystemState{};
}

} // namespace Aqua
