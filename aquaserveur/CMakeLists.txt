idf_component_register(
    SRCS
        "aquaserveur.cpp"

        "core/BootManager.cpp"
        "core/SystemContext.cpp"
        "core/StateManager.cpp"

        "config/ConfigLoader.cpp"
        "config/ConfigValidator.cpp"
        "config/ConfigManager.cpp"

        "comm/CommManager.cpp"
        "comm/CommandManager.cpp"

        "sensors/SensorManager.cpp"

        "outputs/OutputManager.cpp"

        "analysis/AnalysisEngine.cpp"
        "analysis/AdviceEngine.cpp"
        "analysis/RoutineManager.cpp"
        "analysis/ManualTestManager.cpp"

        "support/LogManager.cpp"
        "support/TimeService.cpp"

    INCLUDE_DIRS
        "."
        "core"
        "config"
        "comm"
        "sensors"
        "outputs"
        "analysis"
        "models"
        "support"

     REQUIRES
        arduino
)