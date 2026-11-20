cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(test_ringbuf)

# Il codice da testare NON si copia: si usa lo stesso file scritto nel Modulo 1.
# Se lo correggi la', i test controllano subito la versione corretta.
set(RB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../modulo1/11_buffer_circolare/src)

target_sources(app PRIVATE
	src/main.c              # i test
	${RB_DIR}/ringbuf.c     # il codice sotto test
)

# Dove cercare ringbuf.h quando scrivi #include "ringbuf.h"
target_include_directories(app PRIVATE ${RB_DIR})
