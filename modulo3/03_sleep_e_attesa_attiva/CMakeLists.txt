cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(sleep_e_attesa_attiva)

target_sources(app PRIVATE src/main.c)
