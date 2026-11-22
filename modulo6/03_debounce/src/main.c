/*
 * Esercizio 3 (Modulo 6) - Debounce
 *
 * Teoria: teoria/07_periferiche_driver.md, sezione 5;
 *         teoria/06_tempo_lavoro_differito.md (k_work_delayable).
 *
 * OBIETTIVO
 * Un pulsante meccanico "rimbalza": una pressione produce piu' fronti in pochi
 * millisecondi, e con l'esercizio 2 verrebbe contata piu' volte. Qui ogni pressione
 * deve contare UNA volta sola, con la tecnica classica: interrupt + lavoro ritardato.
 * A ogni pressione valida il LED verde led0 cambia stato.
 *
 * PASSI
 * 1. gpio_dt_spec di sw0 e led0, controlli e configurazione come nell'esercizio 2.
 *    Interrupt su ENTRAMBI i fronti: gpio_pin_interrupt_configure_dt(&btn,
 *    GPIO_INT_EDGE_BOTH). Ci servono anche i rilasci, per sapere quando il pulsante
 *    torna libero.
 * 2. Un lavoro ritardato (Modulo 5):
 *        #define DEBOUNCE_MS 30
 *        static void pulsante_stabile(struct k_work *w);
 *        K_WORK_DELAYABLE_DEFINE(lavoro_debounce, pulsante_stabile);
 * 3. La callback dell'interrupt fa UNA cosa sola: riprogramma il lavoro.
 *        k_work_reschedule(&lavoro_debounce, K_MSEC(DEBOUNCE_MS));
 *    Ogni nuovo fronte (rimbalzo) fa ripartire il conto alla rovescia da capo: il
 *    lavoro scatta solo quando il pin e' rimasto fermo per DEBOUNCE_MS.
 * 4. pulsante_stabile() gira nella work queue di sistema (un thread: qui si puo' fare
 *    tutto). Legge il pin con gpio_pin_get_dt e lo confronta con l'ultimo stato
 *    STABILE (variabile static, parte da 0):
 *      - se e' uguale: era solo un rimbalzo, non fare nulla;
 *      - se e' passato a 1: pressione valida -> contatore++, gpio_pin_toggle_dt(&led),
 *        printk("Pressioni valide: %d\n", contatore);
 *      - se e' passato a 0: printk("Rilasciato\n").
 *    Poi aggiorna lo stato stabile.
 * 5. In main() registra la callback (come nell'esercizio 2), stampa "Pronto: premi sw0"
 *    e termina (return 0): il lavoro lo fanno interrupt e work queue.
 * 6. Prova in Renode a simulare un rimbalzo. Il trucco: fermare il tempo virtuale,
 *    dare piu' comandi e ripartire, cosi' tutti i fronti arrivano "nello stesso istante":
 *        pause
 *        sysbus.gpioPortA.sw0 Press
 *        sysbus.gpioPortA.sw0 Release
 *        sysbus.gpioPortA.sw0 Press
 *        start
 *    Deve comparire UNA sola "Pressioni valide". (Oppure usa la macro "rimbalzo" della
 *    teoria, sezione 11.)
 *
 * OUTPUT ATTESO
 *     Pronto: premi sw0
 *     Pressioni valide: 1        (anche con i rimbalzi; LED acceso)
 *     Rilasciato
 *     Pressioni valide: 2        (LED spento)
 *     Rilasciato
 *
 * CONCETTI NUOVI
 * - Rimbalzo dei contatti e debounce.
 * - k_work_reschedule: se il lavoro e' gia' in attesa, il ritardo riparte da zero.
 *   (k_work_schedule invece non lo sposterebbe: provalo nel bonus.)
 * - Divisione del lavoro: interrupt = "e' successo qualcosa" (in microsecondi);
 *   work queue = "decido cosa e' successo davvero" (in un thread).
 *
 * COME VERIFICARE DA SOLO
 *     .\verifica.ps1 modulo6\03_debounce
 * La verifica simula pressioni con rimbalzi e controlla che ognuna conti una volta.
 *
 * ERRORI TIPICI
 * - Contare nella callback: i rimbalzi contano tutti.
 * - Usare k_work_schedule al posto di k_work_reschedule: con i rimbalzi il lavoro
 *   scatta troppo presto, quando il pin non e' ancora stabile.
 * - Confrontare con l'ultima lettura invece che con l'ultimo stato STABILE: un rimbalzo
 *   1-0-1 sembrerebbe una nuova pressione.
 * - DEBOUNCE_MS troppo grande (es. 500): il pulsante sembra "lento" e le pressioni
 *   rapide si perdono.
 *
 * BONUS (facoltativo)
 * - Sostituisci k_work_reschedule con k_work_schedule e ripeti la prova del rimbalzo.
 * - Pressione lunga: se il pulsante resta premuto piu' di 1 secondo stampa
 *   "Pressione lunga" (un secondo lavoro ritardato, cancellato al rilascio con
 *   k_work_cancel_delayable).
 * - Debounce senza interrupt, per campionamento: k_timer ogni 10 ms e stato accettato
 *   solo dopo 3 letture uguali consecutive (teoria sezione 5). Confronta le due tecniche.
 *
 * Scrivi il tuo codice qui sotto.
 */

#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/printk.h>

#define DEBOUNCE_MS 30

/* TODO: gpio_dt_spec, lavoro ritardato, funzione del lavoro e callback (passi 1-4) */

int main(void)
{
	/* TODO: configurazione e registrazione della callback (passi 1 e 5) */

	return 0;
}
