cmake_minimum_required(VERSION 3.20.0)
# Sensore di temperatura simulato (ADT7420 su bus I2C emulato): vedi comune/ e CORSO.md
include(${CMAKE_CURRENT_LIST_DIR}/../../comune/sensore_sim.cmake)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(sensore_api)

target_sources(app PRIVATE src/main.c ${SENSORE_SIM_SORGENTE})
