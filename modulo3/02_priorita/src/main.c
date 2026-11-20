/*
 * Esercizio 2 (Modulo 3) - Priorita' e starvation
 *
 * Teoria: teoria/04_thread_scheduling.md, sezioni 3 e 4.
 *
 * OBIETTIVO
 * Vedere che lo scheduler esegue SEMPRE il thread pronto piu' importante: un thread
 * di priorita' alta che lavora senza mai fermarsi "affama" (starvation) quelli meno
 * importanti. Capire che k_yield non basta e che bisogna davvero aspettare.
 *
 * PASSI
 * 1. Thread "goloso", priorita' 5 (piu' importante), a ripetizione:
 *      - per 5 volte: lavora 100 ms con k_busy_wait(100000) (attesa ATTIVA: la CPU
 *        resta occupata), incrementa la variabile globale giri_goloso e stampa
 *        "goloso: giro %d";
 *      - stampa "goloso: dormo" e dorme 1000 ms con k_msleep(1000).
 * 2. Thread "gentile", priorita' 7 (meno importante), in ciclo infinito:
 *      - stampa "gentile: goloso ha fatto %d giri" (il valore di giri_goloso);
 *      - dorme 100 ms.
 * 3. Esegui. Gentile stampa SOLO quando goloso dorme: il numero di giri che legge e'
 *    sempre un multiplo di 5. Durante i 5 giri di lavoro gentile non gira mai.
 * 4. Esperimento (poi torna indietro): al posto di k_msleep(1000) in goloso metti un
 *    k_yield() in ogni giro, togli il k_msleep e lascia un ciclo infinito di giri.
 *    Gentile gira ancora? Perche'? (Teoria, sezione 4.)
 * 5. Esperimento (poi torna indietro): scambia le priorita' (goloso 7, gentile 5).
 *    Ora gentile interrompe goloso anche a meta' di un giro: i numeri letti non sono
 *    piu' solo multipli di 5.
 * 6. Rimetti la versione dei passi 1-2 e lancia la verifica.
 *
 * OUTPUT ATTESO
 *     goloso: giro 1
 *     goloso: giro 2
 *     goloso: giro 3
 *     goloso: giro 4
 *     goloso: giro 5
 *     goloso: dormo
 *     gentile: goloso ha fatto 5 giri
 *     gentile: goloso ha fatto 5 giri
 *     ...  (circa 10 righe, poi altri 5 giri)
 *     goloso: giro 6
 *     ...
 *     gentile: goloso ha fatto 10 giri
 *
 * CONCETTI NUOVI
 * - Priorita' preemptive (>= 0): numero piu' piccolo = piu' importante.
 * - Preemption: quando goloso si sveglia, interrompe gentile subito.
 * - Starvation: un thread importante che non aspetta mai blocca gli altri.
 * - k_busy_wait contro k_msleep (approfondito nell'esercizio 3).
 * - Variabile globale condivisa tra thread: qui uno solo scrive e l'altro legge;
 *   nel Modulo 4 vedremo perche' in generale e' pericoloso.
 *
 * COME VERIFICARE DA SOLO
 *     .\verifica.ps1 modulo3\02_priorita
 * Controlla che gentile non legga mai un numero di giri che non sia multiplo di 5.
 *
 * ERRORI TIPICI
 * - Pensare che priorita' 7 sia piu' importante di 5: e' il contrario.
 * - Dimenticare k_msleep in gentile: gentile diventa a sua volta "goloso" verso i
 *   thread ancora meno importanti (idle, logging).
 * - Dare a goloso priorita' 0 o negativa: compete con main e con i thread di sistema.
 *
 * BONUS (facoltativo)
 * - Rendi goloso cooperativo (priorita' -1) e metti in main, dopo un k_msleep(50),
 *   un ciclo di stampe "main: ci sono": main (priorita' 0) riesce a interrompere
 *   goloso durante il lavoro? Perche'?
 * - Stampa la priorita' di ogni thread con k_thread_priority_get(k_current_get()).
 *
 * Scrivi il tuo codice qui sotto.
 */

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

/* volatile: la variabile e' letta da un thread e scritta da un altro (Modulo 1). */
static volatile int giri_goloso;

/* TODO: passi 1 e 2 - le due funzioni e i due K_THREAD_DEFINE */

int main(void)
{
	/* Niente da fare qui: lavorano i thread. */
	return 0;
}
