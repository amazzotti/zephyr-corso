/*
 * Esercizio 3 (Modulo 5) - Work queue di sistema
 *
 * Teoria: teoria/06_tempo_lavoro_differito.md, sezione 6.
 *
 * OBIETTIVO
 * Nell'esercizio 2 hai creato un thread apposta per il lavoro "lungo". Qui usi invece la
 * work queue di sistema, un thread che Zephyr ha gia': la callback del timer consegna un
 * "lavoro" (k_work) e il thread sysworkq lo esegue appena puo'. Scopri anche cosa
 * succede se consegni lo stesso lavoro due volte di fila.
 *
 * PASSI
 * 1. Gia' pronti: K_WORK_DEFINE(lavoro, elabora) e il timer K_TIMER_DEFINE(tick, al_tick, NULL).
 * 2. al_tick() (contesto di interrupt, ogni 1000 ms):
 *        ret1 = k_work_submit(&lavoro);
 *        ret2 = k_work_submit(&lavoro);   (subito di nuovo!)
 *    salva ret1 e ret2 in due variabili globali (volatile).
 * 3. elabora(struct k_work *w) (gira nel thread della work queue):
 *    - eseguiti++;
 *    - stampa "Lavoro n=%u nel thread '%s' (submit: %d, %d)" con
 *          k_thread_name_get(k_current_get())   e i due valori salvati
 *    - k_msleep(50);   (nella work queue si puo' dormire, ma vedi esercizio 5)
 *    - se eseguiti == 5: k_timer_stop(&tick); stampa "Fine".
 * 4. In main(): avvia il timer periodico (prima scadenza 1000 ms, periodo 1000 ms).
 *
 * OUTPUT ATTESO
 *     Work queue di sistema: avvio
 *     Lavoro n=1 nel thread 'sysworkq' (submit: 1, 0)
 *     ...
 *     Lavoro n=5 nel thread 'sysworkq' (submit: 1, 0)
 *     Fine
 *
 * CONCETTI NUOVI
 * - k_work: un "lavoro" da eseguire in un thread. K_WORK_DEFINE(nome, funzione).
 * - k_work_submit: mette il lavoro nella work queue di sistema. Si puo' chiamare da un
 *   interrupt: e' il modo piu' semplice di spostare lavoro fuori dalla ISR.
 * - Valore di ritorno: 1 = messo in coda, 0 = era GIA' in coda (non viene duplicato),
 *   2 = era in esecuzione ed e' stato rimesso in coda. Due submit ravvicinati = UNA
 *   esecuzione: va bene per "aggiorna lo stato", non per "conta ogni evento".
 * - CONFIG_THREAD_NAME=y (nel prj.conf): i thread hanno un nome leggibile.
 *
 * COME VERIFICARE DA SOLO
 *     .\verifica.ps1 modulo5\03_workqueue
 *
 * ERRORI TIPICI
 * - Chiamare elabora() direttamente dalla callback: gira in ISR, e k_msleep fallisce.
 * - Aspettarsi due esecuzioni per due submit.
 * - k_thread_name_get restituisce NULL senza CONFIG_THREAD_NAME: printk di "%s" con NULL.
 *
 * BONUS (facoltativo)
 * - Stampa k_work_busy_get(&lavoro) subito dopo il primo submit (dalla callback salvalo
 *   in una variabile): quali bit vedi? (K_WORK_QUEUED, K_WORK_RUNNING...)
 * - Metti k_msleep(1500) in elabora: ora il lavoro e' ancora in esecuzione quando arriva
 *   il tick successivo. Cosa restituisce il primo submit?
 *
 * Scrivi il tuo codice qui sotto.
 */

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

static volatile int ret1;
static volatile int ret2;
static uint32_t eseguiti;

static void elabora(struct k_work *w)
{
	ARG_UNUSED(w);
	/* TODO */
}

K_WORK_DEFINE(lavoro, elabora);

static void al_tick(struct k_timer *t)
{
	ARG_UNUSED(t);
	/* TODO: due k_work_submit di fila, salva i risultati */
}

K_TIMER_DEFINE(tick, al_tick, NULL);

int main(void)
{
	printk("Work queue di sistema: avvio\n");

	/* TODO: avvia il timer */
	ARG_UNUSED(eseguiti);   /* TODO: togli questa riga quando usi eseguiti */

	return 0;
}
