#pragma once

#include "SystemContext.h"

namespace Aqua {

class StateManager {
public:
    void compute(SystemContext& context) const;
    String buildStateJson(const SystemContext& context) const;
};

} // namespace Aqua
