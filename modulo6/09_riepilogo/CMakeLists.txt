cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(riepilogo)

# Il driver del sensore NON si copia: si usa quello scritto nell'esercizio 7.
# Se lo correggi la', questo esercizio usa subito la versione corretta.
set(BME_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../07_driver_bme280/src)

target_sources(app PRIVATE
	src/main.c
	${BME_DIR}/bme280_corso.c
)

# Dove cercare bme280_corso.h quando scrivi #include "bme280_corso.h"
target_include_directories(app PRIVATE ${BME_DIR})
