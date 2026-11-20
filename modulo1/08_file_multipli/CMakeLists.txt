cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(file_multipli)

# TODO (passo 4 della consegna): aggiungi qui anche src/statistiche.c
target_sources(app PRIVATE src/main.c)
