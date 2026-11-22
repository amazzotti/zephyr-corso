# Aggiunge all'esercizio il sensore simulato. Va incluso PRIMA di find_package(Zephyr):
#
#     include(${CMAKE_CURRENT_LIST_DIR}/../../comune/sensore_sim.cmake)
#     find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
#     ...
#     target_sources(app PRIVATE ${SENSORE_SIM_SORGENTE})

set(SENSORE_SIM_DIR ${CMAKE_CURRENT_LIST_DIR})
list(APPEND EXTRA_DTC_OVERLAY_FILE ${SENSORE_SIM_DIR}/sensore_sim.overlay)
list(APPEND EXTRA_CONF_FILE ${SENSORE_SIM_DIR}/sensore_sim.conf)
set(SENSORE_SIM_SORGENTE ${SENSORE_SIM_DIR}/sensore_sim.c)
