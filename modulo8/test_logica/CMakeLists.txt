cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(test_logica)

# Test della logica pura del progetto: i sorgenti si prendono direttamente da
# modulo8/progetto/src (niente copie). Ogni file viene aggiunto solo se esiste gia',
# cosi' l'applicazione compila anche prima che tu l'abbia scritto.
set(PROG_SRC ${CMAKE_CURRENT_SOURCE_DIR}/../progetto/src)

target_sources(app PRIVATE src/main.c)
target_include_directories(app PRIVATE ${PROG_SRC})

foreach(modulo filtro allarme formato)
	if(EXISTS ${PROG_SRC}/${modulo}.c)
		target_sources(app PRIVATE ${PROG_SRC}/${modulo}.c)
		# HA_FILTRO, HA_ALLARME, HA_FORMATO: per attivare i test nel codice
		string(TOUPPER ${modulo} MODULO)
		target_compile_definitions(app PRIVATE HA_${MODULO}=1)
	endif()
endforeach()
