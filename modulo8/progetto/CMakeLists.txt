cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(nodo_monitoraggio)

# Tutti i file .c nella cartella src/ vengono compilati: per aggiungere un modulo
# (es. src/filtro.c) basta creare il file. CONFIGURE_DEPENDS fa si' che CMake se ne
# accorga da solo alla compilazione successiva.
file(GLOB app_sources CONFIGURE_DEPENDS src/*.c)
target_sources(app PRIVATE ${app_sources})
