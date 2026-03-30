# CMakeList.txt : projet CMake pour aquaserveur, incluez la source et définissez
# la logique spécifique au projet ici.
#

# Ajoutez une source à l'exécutable de ce projet.
add_executable (aquaserveur "aquaserveur.cpp" "aquaserveur.h" "core/BootManager.cpp" "core/SystemContext.h" "core/SystemContext.cpp" "core/StateManager.h" "core/StateManager.cpp" "config/ConfigLoader.h" "config/ConfigLoader.cpp" "config/ConfigValidator.h" "config/ConfigValidator.cpp" "config/ConfigManager.h" "config/ConfigManager.cpp" "comm/CommManager.h")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET aquaserveur PROPERTY CXX_STANDARD 20)
endif()

# TODO: Ajoutez des tests et installez des cibles si nécessaire.
