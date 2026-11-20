/*
 * Esercizio 8 (Modulo 6) - Watchdog
 *
 * Teoria: teoria/07_periferiche_driver.md, sezione 10.
 *
 * OBIETTIVO
 * Avviare il watchdog con un timeout di 2 secondi, nutrirlo regolarmente da un
 * thread "di lavoro" e poi simulare un bug che blocca il thread: il watchdog deve
 * resettare la scheda, e il firmware ripartire da capo.
 *
 * PASSI
 * 1. Prendi il device: DEVICE_DT_GET(DT_ALIAS(watchdog0)). Controlla device_is_ready
 *    ("Errore: watchdog non pronto"). Cerca "watchdog0" in build/zephyr/zephyr.dts:
 *    a quale nodo punta l'alias? A che indirizzo sta la periferica?
 * 2. Configura un canale di timeout:
 *        struct wdt_timeout_cfg cfg = {
 *            .window.min = 0,
 *            .window.max = 2000,
 *            .callback = NULL,
 *            .flags = WDT_FLAG_RESET_SOC,
 *        };
 *        int canale = wdt_install_timeout(wdt, &cfg);
 *    Se canale < 0 stampa "Errore install: %d" e termina.
 * 3. Avvia il watchdog: wdt_setup(wdt, WDT_OPT_PAUSE_HALTED_BY_DBG). Da qui in poi
 *    non si torna indietro. Stampa "Watchdog avviato (2000 ms)".
 * 4. Ciclo "di lavoro" nel main:
 *      - per i = 1..5: k_msleep(500), wdt_feed(wdt, canale), stampa
 *        "Nutro il watchdog (%d)";
 *      - poi stampa "Simulo un blocco!" e blocca il thread per sempre:
 *        k_sleep(K_FOREVER).
 * 5. Esegui con .\esegui.ps1 e guarda la UART: dopo "Simulo un blocco!" passano circa
 *    2 secondi e compare di nuovo il banner "*** Booting Zephyr OS ***". Il firmware
 *    ricomincia: e' un ciclo senza fine (ogni giro finisce con un reset).
 *
 * OUTPUT ATTESO
 *     *** Booting Zephyr OS build ... ***
 *     Watchdog avviato (2000 ms)
 *     Nutro il watchdog (1)
 *     ...
 *     Nutro il watchdog (5)
 *     Simulo un blocco!
 *     *** Booting Zephyr OS build ... ***      <- reset del watchdog
 *     Watchdog avviato (2000 ms)
 *     ...
 *
 * CONCETTI NUOVI
 * - Watchdog: timer hardware che resetta il chip se nessuno lo nutre in tempo.
 * - wdt_install_timeout (canale + timeout + azione) e wdt_setup (avvio).
 * - WDT_FLAG_RESET_SOC: azione allo scadere = reset del chip.
 * - Un blocco "silenzioso" (k_sleep(K_FOREVER), deadlock, attesa su un semaforo che
 *   nessuno da') non produce errori: senza watchdog il dispositivo resterebbe muto
 *   per sempre.
 *
 * COME VERIFICARE DA SOLO
 *     .\verifica.ps1 modulo6\08_watchdog
 * La verifica aspetta il blocco e poi un secondo avvio.
 *
 * ERRORI TIPICI
 * - Nutrire il watchdog da un k_timer: il timer continua a girare anche se il thread
 *   e' bloccato, quindi il reset non arriva mai. Il watchdog lo deve nutrire chi va
 *   sorvegliato.
 * - Usare WDT_OPT_PAUSE_IN_SLEEP: quando il thread si blocca la CPU va a dormire e il
 *   watchdog si ferma con lei. Prova (bonus) e osserva.
 * - Periodo di nutrimento troppo vicino al timeout (es. 1900 ms con 2000): basta un
 *   piccolo ritardo per un reset indesiderato. Regola pratica: nutrire ogni 1/3 - 1/2
 *   del timeout.
 * - Chiamare wdt_setup prima di wdt_install_timeout: errore, i canali vanno installati
 *   prima dell'avvio.
 *
 * BONUS (facoltativo)
 * - Prova WDT_OPT_PAUSE_IN_SLEEP al posto di WDT_OPT_PAUSE_HALTED_BY_DBG: il reset
 *   arriva ancora? Spiega perche' in una riga.
 * - Conta i reset: una variabile marcata __noinit (non azzerata all'avvio) piu' un
 *   "numero magico" per capire se e' valida:
 *       static __noinit uint32_t magico, avvii;
 *       if (magico != 0xC0FFEE) { magico = 0xC0FFEE; avvii = 0; }
 *       avvii++;
 *   e stampa "Avvio numero %u". Funziona in Renode? (Dipende da cosa fa il simulatore
 *   alla RAM durante il reset: annota il risultato.)
 * - Leggi la causa del reset con l'API hwinfo (CONFIG_HWINFO=y,
 *   hwinfo_get_reset_cause e il flag RESET_WATCHDOG) e stampala all'avvio.
 *
 * Scrivi il tuo codice qui sotto.
 */

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/watchdog.h>
#include <zephyr/sys/printk.h>

int main(void)
{
	/* TODO: device, canale, avvio, ciclo di nutrimento e blocco (passi 1-4) */

	return 0;
}
