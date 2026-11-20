/*
 * Esercizio 5 (Modulo 5) - Una work queue propria
 *
 * Teoria: teoria/06_tempo_lavoro_differito.md, sezioni 6 e 8.
 *
 * OBIETTIVO
 * Due lavori: uno VELOCE e urgente (ogni 500 ms, deve partire subito) e uno LENTO
 * (ogni 2 s, dura 1200 ms: immagina una lunga lettura I2C o una scrittura in flash).
 * Se stanno nella stessa work queue, il lento fa aspettare il veloce. Lo misuri, poi
 * sposti il lento in una work queue tutta sua e misuri di nuovo.
 *
 * PASSI
 * 1. Gia' pronti: i due k_work, lo stack e la struct per la coda propria, il timer.
 * 2. al_tick() (ogni 500 ms, contesto di interrupt):
 *    - tick++; ultimo_tick_ms = k_uptime_get_32();
 *    - k_work_submit(&veloce);                       (sempre nella coda di sistema)
 *    - ogni 4 tick (tick % 4 == 1) consegna anche il lento:
 *          #if USA_CODA_PROPRIA
 *              k_work_submit_to_queue(&coda_lenta, &lento);
 *          #else
 *              k_work_submit(&lento);
 *          #endif
 * 3. lavoro_veloce(): n_veloci++; calcola ritardo = k_uptime_get_32() - ultimo_tick_ms;
 *    stampa "Veloce n=%u: ritardo %u ms". Dopo N_VELOCI esecuzioni ferma il timer e
 *    stampa "Fine".
 * 4. lavoro_lento(): stampa "Lento: inizio", k_msleep(1200), stampa "Lento: fine".
 * 5. In main(), SOLO se USA_CODA_PROPRIA e' 1, avvia la coda propria:
 *        struct k_work_queue_config cfg = { .name = "coda_lenta" };
 *        k_work_queue_init(&coda_lenta);
 *        k_work_queue_start(&coda_lenta, stack_coda_lenta,
 *                           K_THREAD_STACK_SIZEOF(stack_coda_lenta), PRIO_CODA_LENTA, &cfg);
 *    poi stampa "Coda per il lavoro lento: propria" (o "di sistema") e avvia il timer.
 * 6. Prima prova con USA_CODA_PROPRIA 0 (con .\esegui.ps1): annota i ritardi e conta
 *    quanti "Veloce" mancano (alcuni tick si perdono: perche'?). Poi metti 1 e verifica.
 *
 * OUTPUT ATTESO con USA_CODA_PROPRIA 0 (esempio)
 *     Veloce n=1: ritardo 0 ms
 *     Lento: inizio
 *     Lento: fine
 *     Veloce n=2: ritardo 200 ms     <- ha aspettato il lento! (e il tick di 1000 ms
 *                                       si e' perso: vedi DOMANDA a)
 *
 * OUTPUT ATTESO con USA_CODA_PROPRIA 1
 *     Coda per il lavoro lento: propria
 *     Veloce n=1: ritardo 0 ms
 *     Lento: inizio
 *     Veloce n=2: ritardo 0 ms
 *     Veloce n=3: ritardo 0 ms
 *     Lento: fine
 *     ...
 *     Veloce n=12: ritardo 0 ms
 *     Fine
 *
 * CONCETTI NUOVI
 * - La work queue di sistema esegue i lavori uno alla volta: un lavoro lento (che dorme
 *   o calcola a lungo) ritarda TUTTI gli altri, compresi quelli dei driver di Zephyr.
 * - Work queue propria: un thread dedicato, con stack e priorita' scelti da te.
 *   k_work_queue_init + k_work_queue_start, poi k_work_submit_to_queue.
 * - #if / #else / #endif: il preprocessore compila solo un ramo (Modulo 1).
 * - ultimo_tick_ms e' uint32_t: su un Cortex-M a 32 bit lettura e scrittura sono
 *   indivisibili; un int64_t no (richiede due accessi).
 *
 * COME VERIFICARE DA SOLO
 *     .\verifica.ps1 modulo5\05_workqueue_propria
 * (con USA_CODA_PROPRIA a 1: nessun ritardo di 100 ms o piu')
 *
 * ERRORI TIPICI
 * - k_work_submit_to_queue su una coda non avviata: il lavoro non parte mai.
 * - Stack della coda troppo piccolo per printk: usa almeno 1024 byte.
 * - Priorita' della coda propria piu' alta di tutto: un lavoro che calcola (senza
 *   dormire) affamerebbe gli altri thread. Qui 10, bassa.
 *
 * DOMANDE (rispondi in fondo al file)
 * a. Con USA_CODA_PROPRIA 0, perche' alcuni "Veloce" vanno persi mentre gira il lento?
 *    (suggerimento: esercizio 3, valore 0 di k_work_submit)
 * b. Che cosa ti aspetti se il lento, invece di dormire 1200 ms, calcolasse per 1200 ms
 *    (k_busy_wait) con la coda propria a priorita' 10? E a priorita' 2?
 *
 * Scrivi il tuo codice qui sotto.
 */

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

/* 0 = lavoro lento nella coda di sistema, 1 = nella coda propria */
#define USA_CODA_PROPRIA 0

#define N_VELOCI         12
#define DIM_STACK_CODA   1024
#define PRIO_CODA_LENTA  10

K_THREAD_STACK_DEFINE(stack_coda_lenta, DIM_STACK_CODA);
static struct k_work_q coda_lenta;

static volatile uint32_t ultimo_tick_ms;
static volatile uint32_t tick;
static uint32_t n_veloci;

static void lavoro_veloce(struct k_work *w)
{
	ARG_UNUSED(w);
	/* TODO */
}

static void lavoro_lento(struct k_work *w)
{
	ARG_UNUSED(w);
	/* TODO */
}

K_WORK_DEFINE(veloce, lavoro_veloce);
K_WORK_DEFINE(lento, lavoro_lento);

static void al_tick(struct k_timer *t)
{
	ARG_UNUSED(t);
	/* TODO */
}

K_TIMER_DEFINE(timer_tick, al_tick, NULL);

int main(void)
{
	/* TODO: avvio della coda propria (solo se USA_CODA_PROPRIA), stampa, avvio timer
	 *       (prima scadenza 500 ms, periodo 500 ms)
	 */
	ARG_UNUSED(coda_lenta);   /* TODO: togli queste righe quando le usi */
	ARG_UNUSED(n_veloci);
	ARG_UNUSED(tick);
	ARG_UNUSED(ultimo_tick_ms);

	return 0;
}

/*
 * RISPOSTE ALLE DOMANDE
 * a.
 * b.
 */
