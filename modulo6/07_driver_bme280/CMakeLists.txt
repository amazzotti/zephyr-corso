cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(driver_bme280)

target_sources(app PRIVATE
	src/main.c
	src/bme280_corso.c      # il TUO driver
)
