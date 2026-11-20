/*
 * Esercizio 3 (Modulo 2) - Logging
 *
 * Teoria: teoria/03_build_kconfig_devicetree.md, sezione 4.
 *
 * OBIETTIVO
 * Usare il logging di Zephyr al posto di printk: messaggi con livello di gravita',
 * nome del modulo e ora; filtrare i messaggi di debug in compilazione; stampare
 * un buffer di byte in esadecimale.
 *
 * PASSI
 * 1. La riga LOG_MODULE_REGISTER(esercizio, LOG_LEVEL_DBG) qui sotto registra il
 *    modulo "esercizio" con livello massimo DEBUG. In main() scrivi, in ordine:
 *        LOG_INF("Avvio");
 *        LOG_DBG("Valore grezzo letto: %d", 512);
 *        LOG_WRN("Temperatura alta: %d", 85);
 *        LOG_ERR("Sensore non risponde");
 *    Esegui e guarda il formato delle righe (ora, livello, modulo) e i colori.
 * 2. Stampa il contenuto dell'array "buffer" (gia' definito sotto) con
 *        LOG_HEXDUMP_INF(buffer, sizeof(buffer), "Buffer ricevuto");
 *    sizeof(buffer) = numero di byte dell'array.
 * 3. Aggiungi DOPO i log una printk("Fine main\n"). Esegui: in che ordine compaiono
 *    printk e log? (Teoria: il logging e' "deferred".)
 * 4. Cambia il livello del modulo in LOG_LEVEL_INF e riesegui: la riga di debug
 *    sparisce. Non e' solo nascosta: non e' proprio nel firmware. Confronta la FLASH
 *    usata nella tabella "Memory region" prima e dopo.
 * 5. Lancia la verifica (con il livello finale INF).
 *
 * OUTPUT ATTESO (finale, ore e colori a parte)
 *     [00:00:00.000,000] <inf> esercizio: Avvio
 *     [00:00:00.000,000] <wrn> esercizio: Temperatura alta: 85
 *     [00:00:00.000,000] <err> esercizio: Sensore non risponde
 *     [00:00:00.000,000] <inf> esercizio: Buffer ricevuto
 *                                         de ad be ef 01 02 03 04 |........
 *     Fine main          (puo' comparire prima o in mezzo ai log)
 *
 * CONCETTI NUOVI
 * - LOG_MODULE_REGISTER(nome, livello): una volta per file .c, fuori dalle funzioni.
 * - Livelli: ERR (errore) < WRN (avviso) < INF (informazione) < DBG (debug).
 *   Il livello del modulo e' il massimo compilato.
 * - Niente "\n" in fondo ai LOG_*: lo aggiunge il logging.
 * - Logging deferred: i messaggi vengono messi in un buffer e stampati da un thread
 *   del logging; chi logga non aspetta la UART (lenta: a 115200 bit/s un carattere
 *   impiega circa 87 microsecondi).
 * - Array di byte uint8_t e notazione esadecimale 0xDE (Modulo 1).
 *
 * COME VERIFICARE DA SOLO
 *     .\verifica.ps1 modulo2\03_logging
 *
 * ERRORI TIPICI
 * - LOG_MODULE_REGISTER dentro main(): errore di compilazione. Va fuori.
 * - Dimenticare CONFIG_LOG=y: i LOG_* non stampano nulla.
 * - Mettere "\n" nei LOG_*: righe vuote in piu'.
 * - Usare LOG_ERR per tutto: il livello serve a chi legge per capire cosa e' grave.
 *
 * BONUS (facoltativo)
 * - In prj.conf aggiungi CONFIG_LOG_MODE_IMMEDIATE=y: ora printk e log escono
 *   nell'ordine in cui sono scritti. Perche'? Che costo ha?
 * - Aggiungi CONFIG_LOG_BACKEND_SHOW_COLOR=n: spariscono i colori.
 * - Prova CONFIG_LOG_DEFAULT_LEVEL e un modulo registrato con LOG_MODULE_REGISTER(x)
 *   senza livello: quale livello prende?
 *
 * Scrivi il tuo codice qui sotto.
 */

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(esercizio, LOG_LEVEL_DBG);

static const uint8_t buffer[] = {0xDE, 0xAD, 0xBE, 0xEF, 0x01, 0x02, 0x03, 0x04};

int main(void)
{
	/* TODO: passi 1, 2 e 3 */

	return 0;
}
