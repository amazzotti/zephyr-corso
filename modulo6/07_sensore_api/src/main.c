/*
 * Esercizio 7 (Modulo 6) - La sensor API
 *
 * Teoria: teoria/07_periferiche_driver.md, sezione 9.
 *
 * OBIETTIVO
 * Leggere lo stesso sensore dell'esercizio 6 (ADT7420) senza toccare un registro: con il
 * driver ufficiale di Zephyr e la sensor API, l'interfaccia uguale per tutti i sensori.
 * Ogni secondo il programma stampa la temperatura.
 *
 * PASSI
 * 1. Prendi il device del sensore:
 *        static const struct device *const sens = DEVICE_DT_GET(DT_NODELABEL(sensore));
 *    e controlla device_is_ready(sens) ("Errore: sensore non pronto"). Il driver lo ha
 *    gia' inizializzato Zephyr all'avvio (ha letto l'ID e scritto la configurazione,
 *    proprio come hai fatto a mano nell'esercizio 6).
 * 2. Stampa "Sensore: %s" con il nome del device (sens->name).
 * 3. Ogni secondo:
 *        struct sensor_value t;
 *        sensor_sample_fetch(sens);                              la misura
 *        sensor_channel_get(sens, SENSOR_CHAN_AMBIENT_TEMP, &t); il valore
 *    Controlla i valori restituiti ("Errore lettura: %d").
 * 4. struct sensor_value e' fatta da due interi: val1 = parte intera, val2 = milionesimi.
 *    22.5 C -> val1 = 22, val2 = 500000. Converti in centesimi (int32_t):
 *        int32_t c100 = t.val1 * 100 + t.val2 / 10000;
 *    e stampa "Temperatura: %d.%02d C" (gestendo il segno, vedi errori tipici).
 * 5. Esegui; nella console scrivi  sim temp 3550  e poi  sim temp -525.
 *
 * OUTPUT ATTESO
 *     Sensore: adt7420@48
 *     Temperatura: 22.50 C
 *     sim: temperatura 35.50 C            (dopo "sim temp 3550")
 *     Temperatura: 35.50 C
 *     sim: temperatura -5.25 C            (dopo "sim temp -525")
 *     Temperatura: -5.25 C
 * (Le righe possono iniziare con il prompt "uart:~$ " della shell.)
 *
 * CONCETTI NUOVI
 * - Driver ufficiale + devicetree: il nodo compatible = "adi,adt7420" basta ad attivarlo
 *   (con CONFIG_SENSOR=y). Il tuo codice non sa ne' l'indirizzo ne' i registri.
 * - sensor_sample_fetch (comunica con il sensore) e sensor_channel_get (prende un valore
 *   gia' misurato): un fetch, poi uno o piu' get.
 * - struct sensor_value: numeri con la virgola rappresentati con due interi.
 * - Portabilita': con un altro sensore di temperatura (per esempio un BME280 sulla
 *   scheda vera) cambia solo il devicetree; questo codice resta identico.
 *
 * COME VERIFICARE DA SOLO
 *     .\verifica.ps1 modulo6\07_sensore_api
 *
 * ERRORI TIPICI
 * - Dimenticare sensor_sample_fetch: sensor_channel_get restituisce sempre la vecchia
 *   misura.
 * - Negativi: -5.25 C e' val1 = -5, val2 = -250000 (ENTRAMBI negativi). printk("%d.%02d",
 *   c100 / 100, c100 % 100) stampa "-5.-25". Stampa il segno a parte e poi i valori
 *   assoluti. E -0.50? val1 = 0: il segno va preso da c100, non da val1.
 * - Stampare "%d.%d" con val1 e val2: per 22.05 C (val2 = 50000) esce "22.50000", che
 *   sembra 22.5. Con val2 serve "%06d"; meglio ancora convertire in centesimi (passo 4).
 *
 * BONUS (facoltativo)
 * - sensor_value_to_double(&t) e printk("%f"): serve CONFIG_CBPRINTF_FP_SUPPORT=y.
 *   Quanto cresce il firmware (tabella "Memory region")?
 * - Un mini-driver tuo: scrivi src/adt7420_corso.c/.h con
 *       int adt7420c_leggi_centesimi(int32_t *c100);
 *   usando i registri come nell'esercizio 6, e confronta i valori con la sensor API.
 * - Leggi un attributo con sensor_attr_get(sens, SENSOR_CHAN_AMBIENT_TEMP,
 *   SENSOR_ATTR_RESOLUTION, &v): che risoluzione usa il driver?
 *
 * Scrivi il tuo codice qui sotto.
 */

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/sys/printk.h>

/* TODO: device del sensore e funzione di stampa con il segno (passi 1 e 4) */

int main(void)
{
	/* TODO: controllo, nome e ciclo di lettura (passi 1-4) */

	return 0;
}
