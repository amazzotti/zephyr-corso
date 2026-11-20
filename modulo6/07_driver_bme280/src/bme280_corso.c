/*
 * bme280_corso.c - Il TUO driver della temperatura del BME280.
 *
 * La consegna completa e' in main.c. Qui ci sono solo le firme e i TODO.
 * Formula e registri: teoria/07_periferiche_driver.md, sezione 8.
 */

#include <errno.h>
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/i2c.h>

#include "bme280_corso.h"

/* TODO: device del bus i2c0 (DEVICE_DT_GET(DT_NODELABEL(i2c0))) */

/* TODO: coefficienti di calibrazione della temperatura (variabili static del file):
 *       uint16_t dig_T1; int16_t dig_T2; int16_t dig_T3;
 */

/* TODO: static int leggi_registri(uint8_t reg, uint8_t *buf, size_t n)
 *       (come nell'esercizio 6: i2c_write + i2c_read, due transazioni separate)
 */

/* TODO: static int scrivi_registro(uint8_t reg, uint8_t valore)
 *       (una sola i2c_write di 2 byte: { reg, valore })
 */

int bme280c_init(void)
{
	/* TODO: passi 2-5 della consegna */
	return -ENOSYS;   /* "non ancora implementata": toglila quando scrivi il codice */
}

int bme280c_leggi_centesimi(int32_t *temp_c100)
{
	/* TODO: passi 6-7 della consegna */
	ARG_UNUSED(temp_c100);
	return -ENOSYS;
}
