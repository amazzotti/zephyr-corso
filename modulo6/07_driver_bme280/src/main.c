/*
 * Esercizio 7 (Modulo 6) - Un driver fatto da me: BME280
 *
 * Teoria: teoria/07_periferiche_driver.md, sezioni 2, 7, 8 e 9;
 *         teoria/02_c_embedded.md (file multipli, static, operatori sui bit).
 *
 * OBIETTIVO
 * Scrivere un piccolo DRIVER per la temperatura del BME280, in un file separato
 * (src/bme280_corso.c), con un'interfaccia semplice (src/bme280_corso.h, gia' pronta):
 *     int bme280c_init(void);
 *     int bme280c_leggi_centesimi(int32_t *temp_c100);
 * Il main lo usa per stampare la temperatura ogni secondo. Lo stesso driver lo riusi
 * nell'esercizio 9 e nel progetto del Modulo 8.
 *
 * PERCHE' UN DRIVER FATTO DA TE (e non quello di Zephyr)
 * Zephyr ha un driver ufficiale del BME280 ("bosch,bme280" nel devicetree +
 * CONFIG_SENSOR=y) che si usa con la SENSOR API (teoria, sezione 9):
 *     sensor_sample_fetch(dev);                               il sensore misura
 *     sensor_channel_get(dev, SENSOR_CHAN_AMBIENT_TEMP, &v);  leggi un canale
 *     struct sensor_value v;  ->  v.val1 = 22, v.val2 = 500000   (22.5 C)
 * Su una scheda vera useresti quello. In Renode pero' non funziona: il driver legge i
 * registri con il "repeated start" (i2c_write_read / i2c_burst_read), che il modello I2C
 * del simulatore non gestisce, e l'inizializzazione fallisce. Scriverlo tu e' anche il
 * modo migliore per capire cosa fa un driver: e' esattamente questo lavoro.
 *
 * PASSI (nel file src/bme280_corso.c)
 * 1. Copia dall'esercizio 6 la funzione leggi_registri(reg, buf, n) (i2c_write del
 *    numero di registro, poi i2c_read), adattata: device del bus e indirizzo
 *    BME280C_INDIRIZZO. Rendila "static": e' un dettaglio interno del driver.
 *    Scrivi anche scrivi_registro(reg, valore): un'unica i2c_write di 2 byte {reg, valore}.
 * 2. bme280c_init(): se il bus non e' pronto (device_is_ready) restituisci -ENODEV.
 * 3. Leggi l'ID (registro 0xD0): se la lettura fallisce restituisci l'errore; se non
 *    vale 0x60 restituisci -ENODEV.
 * 4. Leggi 6 byte da 0x88 e ricomponi dig_T1 (uint16_t), dig_T2 e dig_T3 (int16_t),
 *    little-endian, come nell'esercizio 6. Salvali in variabili "static" del file:
 *    servono a ogni lettura.
 * 5. Avvia le misure continue: scrivi_registro(0xF4, 0x27)
 *    (ctrl_meas = 001 001 11: temperatura x1, pressione x1, modo "normal").
 *    Restituisci 0.
 * 6. bme280c_leggi_centesimi(): leggi 3 byte da 0xFA (temp_msb, temp_lsb, temp_xlsb) e
 *    ricomponi il valore grezzo a 20 bit:
 *        int32_t adc_T = (msb << 12) | (lsb << 4) | (xlsb >> 4);
 * 7. Applica la formula di compensazione del datasheet (teoria, sezione 8), tutta con
 *    int32_t, e scrivi il risultato (centesimi di grado) in *temp_c100:
 *        var1 = ((((adc_T >> 3) - ((int32_t)dig_T1 << 1))) * ((int32_t)dig_T2)) >> 11;
 *        var2 = (((((adc_T >> 4) - ((int32_t)dig_T1)) *
 *                  ((adc_T >> 4) - ((int32_t)dig_T1))) >> 12) * ((int32_t)dig_T3)) >> 14;
 *        t_fine = var1 + var2;
 *        T = (t_fine * 5 + 128) >> 8;
 *    Restituisci 0.
 *
 * PASSI (in questo file, main.c)
 * 8. Chiama bme280c_init(). Se restituisce un errore stampa "Errore init: %d" e termina;
 *    altrimenti stampa "BME280 pronto".
 * 9. Ogni 1000 ms: bme280c_leggi_centesimi(&t). Se fallisce stampa "Errore lettura: %d",
 *    altrimenti stampa la temperatura come "Temperatura: 22.50 C". Niente float: parte
 *    intera t / 100 e parte decimale t % 100 su due cifre (%02d). Per le temperature
 *    negative attento al segno: -125 deve diventare "-1.25" (suggerimento: stampa il
 *    "-" a parte e lavora sul valore assoluto).
 * 10. Esegui con esegui.ps1 e, nel monitor di Renode, cambia la temperatura:
 *         sysbus.twi0.bme280 Temperature 35.5
 *     Entro un paio di secondi deve comparire "Temperatura: 35.50 C".
 *
 * OUTPUT ATTESO (Renode parte con 22.5 C, impostati da renode/esercizio.resc)
 *     BME280 pronto
 *     Temperatura: 22.50 C
 *     Temperatura: 22.50 C
 *     ...                                   (dopo "Temperature 35.5" nel monitor)
 *     Temperatura: 35.50 C
 *
 * Verifica a mano del primo valore: in Renode la calibrazione vale dig_T1 = 32768,
 * dig_T2 = -32768, dig_T3 = 0 e i registri 0xFA-0xFC valgono 71 f0 d0, quindi
 * adc_T = 466701, var1 = 115184, var2 = 0, T = (115184 * 5 + 128) >> 8 = 2250.
 *
 * CONCETTI NUOVI
 * - Un driver = codice che conosce i registri e offre funzioni semplici a chi lo usa.
 * - Interfaccia (.h) separata dall'implementazione (.c); "static" per i dettagli interni.
 * - Codici di errore negativi (-ENODEV, -EIO) propagati verso l'alto.
 * - Modi del BME280: sleep, forced (una misura), normal (misure continue).
 * - Aritmetica intera a virgola fissa: centesimi di grado al posto dei float.
 * - La sensor API di Zephyr: cos'e' e perche' esiste (teoria, sezione 9).
 *
 * COME VERIFICARE DA SOLO
 *     .\verifica.ps1 modulo6\07_driver_bme280
 * La verifica controlla 22.50, cambia la temperatura a 35.5 dal monitor e controlla
 * 35.50.
 *
 * ERRORI TIPICI
 * - Usare i2c_write_read / i2c_burst_read: in Renode leggi dati sbagliati senza errori.
 * - adc_T ricomposto con i byte nell'ordine sbagliato o senza ">> 4" su xlsb.
 * - dig_T2 / dig_T3 come uint16_t: la formula da' valori assurdi.
 * - Dimenticare i cast (int32_t) nella formula: dig_T1 e' unsigned e la sottrazione
 *   diventa senza segno.
 * - Leggere la temperatura subito dopo aver scritto ctrl_meas: la prima misura non e'
 *   ancora pronta. Il main aspetta 1 s prima della prima lettura? Se no, valuta un
 *   k_msleep(100) alla fine di bme280c_init().
 * - Stampare 22.5 come "22.5" invece di "22.50": usa %02d per la parte decimale.
 *
 * BONUS (facoltativo)
 * - Aggiungi al driver int bme280c_leggi(struct sensor_value *v) che riempie val1 (parte
 *   intera) e val2 (milionesimi), come la sensor API: 22.50 C -> val1 = 22,
 *   val2 = 500000. Attento ai negativi (-1.25 -> val1 = -1, val2 = -250000).
 *   (Il tipo e' in <zephyr/drivers/sensor.h>; per usare solo il tipo non serve
 *   CONFIG_SENSOR.)
 * - Leggi anche la pressione (registri 0xF7-0xF9 e coefficienti dig_P1..dig_P9 da
 *   0x8E): la formula e' nel datasheet, sezione 4.2.3. Ti serve t_fine.
 * - Imposta una temperatura negativa dal monitor (Temperature -5.25) e controlla la
 *   stampa del segno.
 *
 * Scrivi il tuo codice qui sotto (e in src/bme280_corso.c).
 */

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

#include "bme280_corso.h"

int main(void)
{
	/* TODO: passi 8-9 */

	return 0;
}
