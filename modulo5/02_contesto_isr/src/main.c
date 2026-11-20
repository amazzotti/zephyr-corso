/*
 * Esercizio 2 (Modulo 5) - Contesto di interrupt
 *
 * Teoria: teoria/06_tempo_lavoro_differito.md, sezioni 2, 3 e 5.
 *
 * OBIETTIVO
 * Toccare con mano la differenza tra codice che gira in un thread e codice che gira in
 * un interrupt. La callback di un k_timer (contesto di interrupt) non puo' bloccarsi:
 * fa il minimo e sveglia un thread con un semaforo; il lavoro vero lo fa il thread.
 * Poi, come esperimento, si fa l'errore apposta e si legge il messaggio di Zephyr.
 *
 * PASSI
 * 1. In main() stampa  "In main: k_is_in_isr = %d"  con k_is_in_isr().
 * 2. Nella callback al_tick() (timer periodico, ogni 300 ms):
 *    - solo alla PRIMA chiamata (usa una variabile static bool prima = true):
 *          printk("Nel callback: k_is_in_isr = %d\n", k_is_in_isr());
 *    - k_sem_give(&tick_sem);   (sveglia il thread)
 *    - #if ESPERIMENTO_SLEEP_IN_ISR  k_msleep(10);  #endif
 * 3. Thread elaboratore(): ciclo
 *    - k_sem_take(&tick_sem, K_FOREVER);
 *    - risvegli++; stampa "Thread svegliato %u volte (k_is_in_isr = %d)"
 *    - k_msleep(50);   (lavoro "lungo": nel thread si puo' dormire!)
 *    - quando risvegli == 5: k_timer_stop(&tick); stampa "Fine"; esci dalla funzione.
 * 4. In main() avvia il timer: k_timer_start(&tick, K_MSEC(300), K_MSEC(300));
 * 5. Esegui e verifica. Poi l'ESPERIMENTO: metti ESPERIMENTO_SLEEP_IN_ISR a 1,
 *    esegui con .\esegui.ps1 e leggi cosa stampa Zephyr. Scrivi in fondo cosa hai visto.
 *    Rimetti 0 prima di fare commit.
 *
 * OUTPUT ATTESO (con ESPERIMENTO_SLEEP_IN_ISR 0)
 *     In main: k_is_in_isr = 0
 *     Nel callback: k_is_in_isr = 1
 *     Thread svegliato 1 volte (k_is_in_isr = 0)
 *     ...
 *     Thread svegliato 5 volte (k_is_in_isr = 0)
 *     Fine
 *
 * OUTPUT ATTESO (con ESPERIMENTO_SLEEP_IN_ISR 1), qualcosa come:
 *     ASSERTION FAIL [!arch_is_in_isr()] @ .../kernel/sched.c:...
 *     ... FATAL ERROR ... (registri della CPU)
 *
 * CONCETTI NUOVI
 * - Contesto di interrupt: il codice non appartiene a un thread. Non si puo' bloccare:
 *   k_msleep, k_mutex_lock, k_sem_take con attesa, sono vietati.
 * - k_is_in_isr(): 1 in contesto di interrupt, 0 in un thread.
 * - Schema top half / bottom half: la ISR da' un semaforo, il thread fa il lavoro.
 * - CONFIG_ASSERT=y (nel prj.conf): attiva i controlli __ASSERT dentro Zephyr. Il kernel
 *   controlla che k_sleep non venga chiamato da un interrupt e, se succede, si ferma
 *   con un messaggio chiaro invece di comportarsi in modo imprevedibile.
 *
 * COME VERIFICARE DA SOLO
 *     .\verifica.ps1 modulo5\02_contesto_isr
 * (con ESPERIMENTO_SLEEP_IN_ISR a 0)
 *
 * ERRORI TIPICI
 * - Stampare dentro la callback a ogni scadenza: funziona, ma riempie la console e
 *   allunga l'interrupt. Una stampa sola basta per vedere il contesto.
 * - k_sem_take(&tick_sem, K_FOREVER) dentro la callback: altro modo per bloccarsi in ISR.
 * - Se il thread impiega piu' di 300 ms, alcuni give vanno "persi" (semaforo con
 *   massimo 1). Qui il thread impiega 50 ms: nessun problema.
 *
 * BONUS (facoltativo)
 * - Fai stampare anche k_current_get() nel main, nel thread e nella callback (%p): cosa
 *   restituisce nella callback? (il thread che e' stato interrotto!)
 * - Senza CONFIG_ASSERT, cosa succede con l'esperimento attivo? (Prova e descrivi.)
 *
 * Scrivi il tuo codice qui sotto.
 */

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

/* Metti 1 per l'esperimento del passo 5. */
#define ESPERIMENTO_SLEEP_IN_ISR 0

#define DIM_STACK 1024

K_SEM_DEFINE(tick_sem, 0, 1);

static void al_tick(struct k_timer *t)
{
	ARG_UNUSED(t);
	/* TODO: stampa alla prima chiamata, k_sem_give, esperimento */
}

K_TIMER_DEFINE(tick, al_tick, NULL);

static void elaboratore(void *p1, void *p2, void *p3)
{
	ARG_UNUSED(p1);
	ARG_UNUSED(p2);
	ARG_UNUSED(p3);

	/* TODO: ciclo: take, stampa, k_msleep(50), stop dopo 5 risvegli */
}

K_THREAD_DEFINE(t_elaboratore, DIM_STACK, elaboratore, NULL, NULL, NULL, 5, 0, 0);

int main(void)
{
	/* TODO: stampa k_is_in_isr() e avvia il timer */

	return 0;
}

/*
 * COSA HO VISTO CON L'ESPERIMENTO:
 *
 */
