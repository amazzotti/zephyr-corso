cmake_minimum_required(VERSION 3.20.0)
# Sensore di temperatura simulato (ADT7420 su bus I2C emulato, comando "sim temp"):
# vedi comune/ e CORSO.md, sezione 3. Va incluso prima di find_package.
include(${CMAKE_CURRENT_LIST_DIR}/../../comune/sensore_sim.cmake)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(nodo_monitoraggio)

# Tutti i file .c nella cartella src/ vengono compilati: per aggiungere un modulo
# (es. src/filtro.c) basta creare il file. CONFIGURE_DEPENDS fa si' che CMake se ne
# accorga da solo alla compilazione successiva.
file(GLOB app_sources CONFIGURE_DEPENDS src/*.c)
target_sources(app PRIVATE ${app_sources} ${SENSORE_SIM_SORGENTE})
