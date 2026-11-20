/*
 * Esercizio 5 (Modulo 3) - Gli stati di un thread
 *
 * Teoria: teoria/04_thread_scheduling.md, sezioni 5 e 10.
 *
 * OBIETTIVO
 * Elencare tutti i thread del sistema con nome, priorita' e stato; sospendere e
 * riprendere un thread e vedere il cambio di stato.
 *
 * PASSI
 * 1. Crea con K_THREAD_DEFINE un thread chiamato "lavoratore" (priorita' 5) che in un
 *    ciclo infinito incrementa un contatore, stampa "lavoratore: contatore = %d" e
 *    dorme 300 ms. Con CONFIG_THREAD_NAME il nome del thread e' il primo argomento di
 *    K_THREAD_DEFINE: chiamalo proprio  lavoratore.
 * 2. Completa la funzione stampa_thread() qui sotto: viene chiamata da k_thread_foreach
 *    per OGNI thread del sistema. Deve stampare una riga:
 *        "  %-12s prio %3d  stato %s"
 *    con il nome  k_thread_name_get(t)  (puo' essere NULL: in quel caso stampa "?"),
 *    la priorita'  k_thread_priority_get(t)  e lo stato
 *        k_thread_state_str(t, buf, sizeof(buf))   (buf: char buf[32] locale).
 *    %-12s = stringa allineata a sinistra su 12 caratteri, %3d = intero su 3 caratteri.
 * 3. In main():
 *      - k_msleep(1000) per lasciar lavorare il lavoratore;
 *      - stampa "main: sospendo lavoratore" e chiama k_thread_suspend(id_lavoratore)
 *        (id_lavoratore e' il primo argomento di K_THREAD_DEFINE);
 *      - stampa "Elenco thread:" e poi k_thread_foreach(stampa_thread, NULL);
 *      - k_msleep(1000): in questo secondo il lavoratore NON stampa;
 *      - stampa "main: riprendo lavoratore" e k_thread_resume(id_lavoratore);
 *      - k_msleep(1000) e poi ritorna.
 * 4. Esegui. Nell'elenco: quali thread esistono anche se non li hai creati tu? In che
 *    stato e' main mentre stampa? E idle?
 * 5. Lancia la verifica.
 *
 * OUTPUT ATTESO (simile, l'ordine dell'elenco puo' variare)
 *     lavoratore: contatore = 1
 *     lavoratore: contatore = 2
 *     ...
 *     main: sospendo lavoratore
 *     Elenco thread:
 *       lavoratore   prio   5  stato suspended
 *       main         prio   0  stato queued
 *       idle         prio  15  stato queued
 *       sysworkq     prio  -1  stato pending
 *     main: riprendo lavoratore
 *     lavoratore: contatore = 5
 *
 * CONCETTI NUOVI
 * - Stati: pronto (queued), in esecuzione, in attesa (pending/sleeping), sospeso
 *   (suspended), terminato (dead). Le parole esatte dipendono dalla versione.
 * - k_thread_suspend / k_thread_resume: togliere e rimettere un thread nello scheduling.
 * - k_thread_foreach(funzione, dato): chiama una funzione per ogni thread.
 *   E' un esempio di "callback": passi una funzione a un'altra funzione.
 * - k_tid_t: l'identificatore di un thread (in realta' un puntatore a struct k_thread).
 *
 * COME VERIFICARE DA SOLO
 *     .\verifica.ps1 modulo3\05_stati_thread
 *
 * ERRORI TIPICI
 * - Stampare k_thread_name_get(t) senza controllare NULL: alcuni thread possono non
 *   avere nome; printk con %s e NULL stampa "(null)" o peggio.
 * - Sospendere main stesso: k_thread_suspend(k_current_get()) in main e poi nessuno lo
 *   riprende piu'.
 * - Dimenticare CONFIG_THREAD_MONITOR: k_thread_foreach non elenca nulla (o non compila).
 *
 * BONUS (facoltativo)
 * - Usa il parametro "user_data" di k_thread_foreach per contare i thread: passa
 *   l'indirizzo di un int e incrementalo nella callback. Stampa "Thread totali: %d".
 * - Stampa anche l'indirizzo e la dimensione dello stack di ogni thread
 *   (t->stack_info.start, t->stack_info.size).
 *
 * Scrivi il tuo codice qui sotto.
 */

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

/* TODO: passo 1 - funzione del lavoratore e K_THREAD_DEFINE */

static void stampa_thread(const struct k_thread *cthread, void *user_data)
{
	/* k_thread_foreach passa un puntatore const; le funzioni k_thread_*_get vogliono
	 * un k_tid_t (non const): convertiamo una volta qui. */
	k_tid_t t = (k_tid_t)cthread;

	ARG_UNUSED(user_data);
	ARG_UNUSED(t);

	/* TODO: passo 2 */
}

int main(void)
{
	/* TODO: passo 3 */
	ARG_UNUSED(stampa_thread);

	return 0;
}
