/*
 * Esercizio 6 (Modulo 5) - Precisione: deriva e jitter
 *
 * Teoria: teoria/06_tempo_lavoro_differito.md, sezioni 1, 9 e 10.
 *
 * OBIETTIVO
 * Un compito periodico ogni 100 ms che lavora per 7 ms. Lo scrivi in due modi e misuri
 * quanto tempo impiegano 50 periodi (l'ideale e' 5000 ms):
 *   A. ciclo "ingenuo": lavora, poi k_msleep(100)
 *   B. timer periodico: k_timer_status_sync() aspetta la scadenza, poi lavora
 * Nel caso A l'errore si accumula (deriva), nel caso B no.
 *
 * PASSI
 * 1. lavoro_simulato() e' gia' pronta: occupa la CPU per 7 ms.
 * 2. Ciclo A in main():
 *        int64_t inizio = k_uptime_get();
 *        per 50 volte: lavoro_simulato(); k_msleep(PERIODO_MS);
 *        int64_t durata_a = k_uptime_get() - inizio;
 *    stampa "Ciclo con k_msleep: 50 periodi in %lld ms (ideale 5000, deriva %lld ms)"
 *    (deriva = durata - 5000).
 * 3. Ciclo B:
 *        k_timer_start(&periodo, K_MSEC(PERIODO_MS), K_MSEC(PERIODO_MS));
 *        inizio = k_uptime_get();
 *        per 50 volte: k_timer_status_sync(&periodo); lavoro_simulato();
 *        durata_b = k_uptime_get() - inizio;  k_timer_stop(&periodo);
 *    stessa stampa con "Ciclo con k_timer".
 *    (Nota: durata_b comprende il lavoro dell'ultimo periodo, circa 7 ms in piu'.)
 * 4. Durante il ciclo B misura il JITTER: a ogni giro, subito dopo il sync, calcola
 *        scarto = (k_uptime_get() - inizio) - PERIODO_MS * (i + 1)
 *    e tieni il massimo. Stampa "Jitter massimo k_timer: %lld ms".
 * 5. Stampa "Deriva minore con k_timer: si" se la deriva B < deriva A, altrimenti "no".
 *
 * OUTPUT ATTESO (numeri indicativi)
 *     Misura della deriva su 50 periodi da 100 ms
 *     Ciclo con k_msleep: 50 periodi in 5352 ms (ideale 5000, deriva 352 ms)
 *     Ciclo con k_timer: 50 periodi in 5007 ms (ideale 5000, deriva 7 ms)
 *     Jitter massimo k_timer: 0 ms
 *     Deriva minore con k_timer: si
 *
 * CONCETTI NUOVI
 * - Deriva (drift): errore che si accumula a ogni periodo. Con k_msleep il periodo vero
 *   e' 100 ms + tempo di lavoro + arrotondamenti dei tick.
 * - k_timer periodico: le scadenze sono fissate fin dall'inizio, il lavoro non le sposta.
 * - k_timer_status_sync: blocca il thread fino alla prossima scadenza.
 * - Jitter: quanto varia il ritardo di ogni singolo risveglio. Non si accumula.
 * - Tempo virtuale di Renode: k_uptime_get misura il tempo della scheda simulata, quindi
 *   la deriva si vede davvero; ma Renode puo' impiegare piu' o meno di 5 s "veri" per
 *   simulare questi 5 s. Non cronometrare con il telefono!
 *
 * COME VERIFICARE DA SOLO
 *     .\verifica.ps1 modulo5\06_jitter
 *
 * ERRORI TIPICI
 * - Stampare un int64_t con %d: valori senza senso. Usa %lld.
 * - Dimenticare k_timer_stop alla fine: il timer continua a scadere per sempre.
 * - Mettere k_timer_start dopo "inizio = k_uptime_get()" o viceversa con altro codice in
 *   mezzo: falsa la misura.
 *
 * BONUS (facoltativo)
 * - Terza variante C: k_sleep(K_TIMEOUT_ABS_MS(prossimo)) con prossimo += PERIODO_MS a
 *   ogni giro (dormire fino a un istante assoluto). Che deriva ottieni?
 * - Cosa succede nel ciclo B se il lavoro dura 130 ms, piu' del periodo? Guarda il
 *   valore restituito da k_timer_status_sync (scadenze perse).
 *
 * Scrivi il tuo codice qui sotto.
 */

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

#define PERIODO_MS 100
#define N_PERIODI  50
#define LAVORO_US  7000

K_TIMER_DEFINE(periodo, NULL, NULL);

/* Occupa la CPU per LAVORO_US microsecondi (lavoro "vero", non sleep). */
static void lavoro_simulato(void)
{
	k_busy_wait(LAVORO_US);
}

int main(void)
{
	printk("Misura della deriva su %d periodi da %d ms\n", N_PERIODI, PERIODO_MS);

	/* TODO: ciclo A con k_msleep */

	/* TODO: ciclo B con k_timer, misura del jitter */

	/* TODO: confronto */

	ARG_UNUSED(lavoro_simulato);   /* TODO: togli questa riga quando la usi */

	return 0;
}
