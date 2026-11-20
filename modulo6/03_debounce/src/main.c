/*
 * Esercizio 3 (Modulo 6) - Debounce
 *
 * Teoria: teoria/07_periferiche_driver.md, sezioni 5 e 11 (macro di Renode);
 *         teoria/06_tempo_lavoro_differito.md (k_work_delayable).
 *
 * OBIETTIVO
 * Contare SOLO le pressioni vere di sw0, ignorando i rimbalzi meccanici: una pressione
 * con rimbalzi (pin che oscilla per qualche ms) deve contare 1.
 *
 * PASSI
 * 1. Parti dall'esercizio 2 (pulsante, LED, callback). Questa volta l'interrupt va
 *    su ENTRAMBI i fronti: GPIO_INT_EDGE_BOTH (vogliamo sapere anche quando finisce
 *    di rimbalzare dopo un rilascio).
 * 2. Definisci un lavoro ritardato e la sua funzione:
 *        static void debounce_fn(struct k_work *work);
 *        static K_WORK_DELAYABLE_DEFINE(debounce_work, debounce_fn);
 *        #define DEBOUNCE_MS 30
 * 3. La callback dell'interrupt fa UNA sola cosa:
 *        k_work_reschedule(&debounce_work, K_MSEC(DEBOUNCE_MS));
 *    A ogni fronte il conto alla rovescia riparte: debounce_fn viene eseguita solo
 *    quando il pin e' rimasto fermo per 30 ms.
 * 4. In debounce_fn (gira nel thread della work queue di sistema, non in interrupt):
 *      - leggi lo stato stabile con gpio_pin_get_dt(&btn);
 *      - tieni in una variabile static lo stato stabile precedente;
 *      - se ora e' premuto e prima non lo era: incrementa il contatore, inverti led0 e
 *        stampa "Pressioni valide: N";
 *      - se ora e' rilasciato e prima era premuto: stampa "Rilasciato".
 * 5. Stampa "Pronto: premi sw0" alla fine della configurazione in main(). Il main
 *    puo' poi terminare con return 0: il lavoro lo fanno callback e work queue.
 * 6. ESPERIMENTO in Renode (.\esegui.ps1): definisci nel monitor la macro "rimbalzo"
 *    della teoria (sezione 11) e lanciala con runMacro $rimbalzo, poi
 *        sysbus.gpio0.sw0 Release
 *    Quante pressioni conta questo programma? E quello dell'esercizio 2 con la stessa
 *    macro? Scrivi i risultati in fondo a questo commento.
 *
 * OUTPUT ATTESO (due pressioni pulite, oppure due pressioni con rimbalzi)
 *     Pronto: premi sw0
 *     Pressioni valide: 1
 *     Rilasciato
 *     Pressioni valide: 2
 *     Rilasciato
 *
 * CONCETTI NUOVI
 * - Rimbalzo dei contatti e perche' un interrupt sul fronte conta troppo.
 * - k_work_reschedule: (ri)programma il lavoro facendo ripartire il ritardo; si puo'
 *   chiamare dall'interrupt.
 * - Stato "stabile": si decide solo dopo un periodo di quiete del segnale.
 * - Il thread principale non serve: la logica vive tutta in callback + work queue.
 *
 * COME VERIFICARE DA SOLO
 *     .\verifica.ps1 modulo6\03_debounce
 * La verifica fa due pressioni pulite (in Renode la durata dei rimbalzi in tempo
 * virtuale non si controlla con precisione: i rimbalzi li provi a mano, passo 6).
 *
 * ERRORI TIPICI
 * - k_work_schedule invece di k_work_reschedule: se il lavoro e' gia' programmato
 *   NON riparte il conto alla rovescia, quindi decidi durante i rimbalzi.
 * - Contare nella callback: torni al problema di partenza.
 * - Contare anche il rilascio come pressione (manca il confronto con lo stato precedente).
 * - DEBOUNCE_MS troppo grande (es. 500): due pressioni rapide vere diventano una.
 *
 * BONUS (facoltativo)
 * - Riconosci la pressione LUNGA: se il pulsante resta premuto piu' di 1 s stampa
 *   "Pressione lunga" (un secondo k_work_delayable programmato alla pressione e
 *   cancellato con k_work_cancel_delayable al rilascio).
 * - Rendi il debounce una piccola "libreria" (debounce.c/.h) riusabile per piu' pulsanti:
 *   ti servira' nel Modulo 8.
 *
 * RISULTATI DELL'ESPERIMENTO (passo 6):
 *   esercizio 2 con la macro rimbalzo: ...
 *   esercizio 3 con la macro rimbalzo: ...
 *
 * Scrivi il tuo codice qui sotto.
 */

#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/printk.h>

/* TODO: gpio_dt_spec, lavoro ritardato, callback (passi 1-4) */

int main(void)
{
	/* TODO: configurazione di pulsante, LED e interrupt (passi 1 e 5) */

	return 0;
}
