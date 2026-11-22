/*
 * Pannello di controllo del sensore simulato (comune agli esercizi che usano il sensore).
 *
 * Non fa parte degli esercizi: e' il "banco di prova" che sostituisce il sensore vero.
 * - All'avvio imposta la temperatura simulata a 22.50 C.
 * - Aggiunge alla shell il comando:
 *       sim temp <centesimi>     es.  sim temp 3550  ->  35.50 C
 *       sim temp                 mostra il valore impostato
 *
 * Funziona con l'emulatore dell'ADT7420 di Zephyr (vedi sensore_sim.overlay): il valore
 * finisce nei registri del sensore emulato, e il driver o il tuo codice lo leggono via I2C
 * esattamente come da un sensore vero.
 */

#include <stdlib.h>
#include <zephyr/kernel.h>
#include <zephyr/init.h>
#include <zephyr/drivers/emul.h>
#include <zephyr/drivers/emul_sensor.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/shell/shell.h>

#define TEMP_INIZIALE_C100 2250

static int32_t temp_c100 = TEMP_INIZIALE_C100;

static int imposta(int32_t c100)
{
	const struct emul *e = EMUL_DT_GET(DT_NODELABEL(sensore));
	const int8_t shift = 8; /* valori fino a +-256 gradi */
	q31_t v = (q31_t)(((int64_t)c100 * (1LL << (31 - shift))) / 100);
	struct sensor_chan_spec ch = {.chan_type = SENSOR_CHAN_AMBIENT_TEMP, .chan_idx = 0};
	int r = emul_sensor_backend_set_channel(e, ch, &v, shift);

	if (r == 0) {
		temp_c100 = c100;
	}
	return r;
}

static int cmd_temp(const struct shell *sh, size_t argc, char **argv)
{
	if (argc == 2) {
		int r = imposta((int32_t)strtol(argv[1], NULL, 10));

		if (r != 0) {
			shell_error(sh, "errore %d", r);
			return r;
		}
	}
	/* Segno stampato a parte: con -0.50 la parte intera sarebbe 0 e il "-" si perderebbe. */
	shell_print(sh, "sim: temperatura %s%d.%02d C", temp_c100 < 0 ? "-" : "",
		    abs(temp_c100) / 100, abs(temp_c100) % 100);
	return 0;
}

SHELL_STATIC_SUBCMD_SET_CREATE(sub_sim,
	SHELL_CMD_ARG(temp, NULL, "temperatura simulata in centesimi di grado (es. 3550)",
		      cmd_temp, 1, 1),
	SHELL_SUBCMD_SET_END);
SHELL_CMD_REGISTER(sim, &sub_sim, "Comandi del sensore simulato", NULL);

static int sim_init(void)
{
	return imposta(TEMP_INIZIALE_C100);
}

SYS_INIT(sim_init, APPLICATION, 0);
