/*
 * Esercizio 7 (Modulo 6) - Sensor API
 *
 * Teoria: teoria/07_periferiche_driver.md, sezioni 2, 9 e 11.
 *
 * OBIETTIVO
 * Leggere temperatura, pressione e umidita' dal BME280 ogni 2 secondi usando il driver
 * di Zephyr: niente registri, niente formule di calibrazione. Confrontare la quantita'
 * di codice con l'esercizio 6.
 *
 * PASSI
 * 1. Copia in app.overlay il nodo del BME280 dell'esercizio 6.
 * 2. Prendi il device: DEVICE_DT_GET(DT_NODELABEL(bme280)). Controlla
 *    device_is_ready(): se e' falso stampa "Errore: sensore non pronto" e termina.
 *    (Falso vuol dire che l'init del driver e' fallito: per esempio il chip ID letto
 *    non era quello atteso.) Altrimenti stampa "Sensore %s pronto" con dev->name.
 * 3. Ciclo infinito ogni 2 s:
 *      - sensor_sample_fetch(dev): se < 0 stampa "Errore fetch: %d" e salta il giro;
 *      - sensor_channel_get per SENSOR_CHAN_AMBIENT_TEMP, SENSOR_CHAN_PRESS e
 *        SENSOR_CHAN_HUMIDITY in tre struct sensor_value;
 *      - stampa con soli interi (val1 = parte intera, val2 = milionesimi):
 *            "Temperatura: %d.%06d C"
 *            "Pressione: %d.%06d kPa"
 *            "Umidita: %d.%06d %%"
 *        (in printk "%%" stampa il carattere %).
 * 4. Attenzione ai negativi: -0,25 C e' val1 = 0, val2 = -250000. Scrivi una funzione
 *        static void stampa_valore(const char *nome, const struct sensor_value *v,
 *                                  const char *unita)
 *    che gestisce il segno: se val1 < 0 o val2 < 0 stampa "-" e poi i valori assoluti.
 *    Usala nel passo 3.
 * 5. Esegui con .\esegui.ps1. Nel monitor di Renode scrivi
 *        sysbus.twi0.bme280
 *    per vedere proprieta' e metodi del sensore simulato. Trova come cambiare la
 *    temperatura (per esempio a 31.5) e controlla che il firmware la legga.
 *    Annota il comando in fondo a questo commento: ti servira' nei prossimi esercizi.
 *
 * OUTPUT ATTESO (i valori dipendono dal sensore simulato)
 *     Sensore bme280@76 pronto
 *     Temperatura: 25.000000 C
 *     Pressione: 101.325000 kPa
 *     Umidita: 50.000000 %
 *     ... (ogni 2 secondi)
 *
 * CONCETTI NUOVI
 * - Sensor API: un'interfaccia unica per tutti i sensori di Zephyr.
 * - fetch (comunicazione con il sensore) e get (lettura di un canale gia' misurato).
 * - Canali: SENSOR_CHAN_AMBIENT_TEMP, SENSOR_CHAN_PRESS, SENSOR_CHAN_HUMIDITY.
 * - struct sensor_value: numeri con la virgola rappresentati con due interi.
 * - Il driver si attiva dal devicetree (compatible) + Kconfig (CONFIG_SENSOR).
 *
 * COME VERIFICARE DA SOLO
 *     .\verifica.ps1 modulo6\07_sensore_api
 * La verifica controlla il formato delle righe, non i valori.
 *
 * ERRORI TIPICI
 * - device_is_ready falso: nodo mancante o sbagliato nell'overlay, oppure indirizzo
 *   diverso da 0x76. Con  logLevel 0 sysbus.twi0  nel monitor vedi cosa succede sul bus.
 * - Chiamare sensor_channel_get senza aver fatto prima sensor_sample_fetch: ottieni
 *   l'ultimo valore misurato (o zero), non uno nuovo.
 * - Stampare val2 con %d invece di %06d: 25.5 C diventa "25.500000" ma 25.05 C diventa
 *   "25.50000" (manca lo zero!).
 * - Usare %f senza CONFIG_CBPRINTF_FP_SUPPORT: printk stampa "%f" letteralmente.
 *
 * BONUS (facoltativo)
 * - Aggiungi CONFIG_CBPRINTF_FP_SUPPORT=y a prj.conf e stampa con
 *   sensor_value_to_double() e "%.2f". Di quanti byte cresce la FLASH? (tabella
 *   "Memory region" a fine compilazione, prima e dopo).
 * - Leggi con CONFIG_SENSOR_LOG_LEVEL_DBG=y e CONFIG_LOG=y i messaggi del driver
 *   durante l'inizializzazione: cosa fa all'avvio?
 * - Apri il sorgente del driver (C:\zephyrproject\zephyr\drivers\sensor\bosch\bme280\)
 *   e trova dove legge il registro 0xD0 e dove applica la formula di compensazione
 *   che hai visto nell'esercizio 6.
 *
 * COMANDO RENODE PER CAMBIARE LA TEMPERATURA (passo 5):
 *   ...
 *
 * Scrivi il tuo codice qui sotto.
 */

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/sys/printk.h>

/* TODO: funzione stampa_valore (passo 4) */

int main(void)
{
	/* TODO: device del sensore e ciclo di lettura (passi 2-3) */

	return 0;
}
