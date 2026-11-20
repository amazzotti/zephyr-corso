/*
 * bme280_corso.h - Un piccolo driver per la temperatura del BME280, scritto nel corso.
 *
 * Interfaccia (gia' completa: NON cambiarla, la riusano modulo6/09_riepilogo e il
 * progetto del Modulo 8).
 */

#ifndef BME280_CORSO_H
#define BME280_CORSO_H

#include <stdint.h>

/* Indirizzo I2C a 7 bit del sensore (pin SDO a massa). */
#define BME280C_INDIRIZZO 0x76

/*
 * Prepara il sensore: controlla il bus, legge e verifica l'ID (0x60), legge i
 * coefficienti di calibrazione della temperatura e avvia le misure continue
 * (ctrl_meas = 0x27).
 * Restituisce 0 se tutto va bene, un codice di errore negativo altrimenti
 * (-ENODEV se il bus non e' pronto o l'ID e' sbagliato, l'errore I2C se la
 * comunicazione fallisce).
 */
int bme280c_init(void);

/*
 * Legge la temperatura piu' recente e la scrive in *temp_c100, in centesimi di grado
 * Celsius (2250 = 22.50 C, -125 = -1.25 C).
 * Restituisce 0 se tutto va bene, un codice di errore negativo altrimenti.
 * Va chiamata dopo bme280c_init().
 */
int bme280c_leggi_centesimi(int32_t *temp_c100);

#endif /* BME280_CORSO_H */
