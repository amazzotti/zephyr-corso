/*
 * Esercizio 8 (Modulo 2) - Riepilogo: lampeggio configurabile
 *
 * Teoria: teoria/03_build_kconfig_devicetree.md, tutto (sezione 10 per il riassunto).
 *
 * OBIETTIVO
 * Mettere insieme gli strumenti del modulo: un LED che lampeggia con
 *   - il LED scelto dal devicetree (alias "lampeggio" nell'overlay -> LED 1),
 *   - il periodo scelto da Kconfig (opzione tua CORSO_PERIODO_MS),
 *   - i messaggi scritti con il logging.
 * Il codice non contiene ne' numeri di pin ne' tempi: solo nomi.
 *
 * PASSI
 * 1. File Kconfig: definisci CORSO_PERIODO_MS (istruzioni nel file).
 * 2. prj.conf: imposta CONFIG_CORSO_PERIODO_MS=800.
 * 3. app.overlay: alias lampeggio = &led1.
 * 4. Nel codice:
 *      - registra il modulo di log "riepilogo" con livello INF;
 *      - crea la gpio_dt_spec da DT_ALIAS(lampeggio);
 *      - controlla che sia pronta e configurala come uscita spenta; in caso di errore
 *        usa LOG_ERR e termina;
 *      - stampa con LOG_INF "Periodo: %d ms" (il valore di Kconfig);
 *      - ciclo infinito: accendi -> LOG_INF("LED acceso") -> k_msleep(periodo)
 *                        spegni  -> LOG_INF("LED spento") -> k_msleep(periodo)
 * 5. Ricompila con -Pulito, esegui, controlla il LED in Renode (sysbus.gpio0.led1 State).
 * 6. Prova a cambiare solo prj.conf (es. 300) o solo l'overlay (&led3): il codice non
 *    cambia. Rimetti 800 e &led1 prima della verifica.
 * 7. Lancia la verifica.
 *
 * OUTPUT ATTESO
 *     [..] <inf> riepilogo: Periodo: 800 ms
 *     [..] <inf> riepilogo: LED acceso
 *     [..] <inf> riepilogo: LED spento
 *     ...
 *
 * CONCETTI NUOVI
 * - Nessuno: e' un esercizio di sintesi. Separare "cosa fa il codice" (main.c),
 *   "con quali parametri software" (Kconfig/prj.conf) e "su quale hardware"
 *   (devicetree/overlay) e' il modo di lavorare di Zephyr.
 *
 * COME VERIFICARE DA SOLO
 *     .\verifica.ps1 modulo2\08_riepilogo -Pulito
 *
 * ERRORI TIPICI
 * - Uno dei tre file non aggiornato: leggi il primo errore, ti dice se manca
 *   CONFIG_CORSO_PERIODO_MS (Kconfig/prj.conf) o DT_N_ALIAS_lampeggio (overlay).
 * - Log scritto prima di cambiare il LED: la verifica legge lo stato appena vede
 *   la riga.
 *
 * BONUS (facoltativo)
 * - Aggiungi un'opzione bool CORSO_LAMPEGGIO_DEBUG: se attiva, stampa con LOG_DBG
 *   anche il numero del giro (ricorda di alzare il livello del modulo).
 * - Aggiungi al nodo zephyr,user dell'overlay una proprieta' "ripetizioni" e fai
 *   lampeggiare il LED solo quel numero di volte.
 *
 * Scrivi il tuo codice qui sotto.
 */

#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>

/* TODO: registrazione del modulo di log e gpio_dt_spec */

int main(void)
{
	/* TODO */

	return 0;
}
