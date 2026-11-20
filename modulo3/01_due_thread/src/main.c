/*
 * Esercizio 1 (Modulo 3) - Due thread
 *
 * Teoria: teoria/04_thread_scheduling.md, sezioni 1, 2 e 10.
 *
 * OBIETTIVO
 * Creare due thread con K_THREAD_DEFINE che stampano a ritmi diversi, ciascuno con il
 * suo contatore, e vedere che main() puo' terminare mentre gli altri continuano.
 *
 * PASSI
 * 1. Scrivi la funzione del thread A:
 *        void thread_a(void *p1, void *p2, void *p3)
 *    con un contatore locale che parte da 0 e un ciclo infinito che stampa
 *    "A: %d" e poi dorme 500 ms (k_msleep(500)).
 *    I tre parametri non servono: scrivi ARG_UNUSED(p1); ecc. per evitare gli avvisi.
 * 2. Scrivi thread_b: uguale ma stampa "B: %d" ogni 1200 ms.
 * 3. Crea i due thread con K_THREAD_DEFINE (fuori dalle funzioni):
 *        K_THREAD_DEFINE(id_a, 1024, thread_a, NULL, NULL, NULL, 5, 0, 0);
 *    e lo stesso per B (id_b, priorita' 5).
 * 4. In main() stampa "main: avvio" e poi "main: finito" e ritorna.
 * 5. Esegui e osserva: main finisce, ma A e B continuano. Conta: quante righe A
 *    compaiono tra due righe B? E' quello che ti aspetti da 1200 / 500?
 * 6. Lancia la verifica.
 *
 * OUTPUT ATTESO (l'ordine delle prime righe puo' variare)
 *     A: 0
 *     B: 0
 *     main: avvio
 *     main: finito
 *     A: 1
 *     A: 2
 *     B: 1
 *     A: 3
 *     ...
 *
 * CONCETTI NUOVI
 * - Thread: funzione + stack + priorita'. Ognuno ha le SUE variabili locali (sul suo
 *   stack): il contatore di A e quello di B sono indipendenti.
 * - K_THREAD_DEFINE: crea il thread in compilazione e lo fa partire all'avvio
 *   (ultimo argomento = ritardo di partenza in ms).
 * - Un solo core: A e B si alternano. Mentre uno dorme (k_msleep), l'altro puo' girare;
 *   quando dormono entrambi gira il thread idle, che mette la CPU in risparmio
 *   energetico.
 * - main e' anch'esso un thread; se ritorna, termina solo lui.
 *
 * COME VERIFICARE DA SOLO
 *     .\verifica.ps1 modulo3\01_due_thread
 *
 * ERRORI TIPICI
 * - Firma della funzione sbagliata (es. void thread_a(void)): errore o avviso sui tipi.
 *   Deve essere esattamente (void *p1, void *p2, void *p3).
 * - Contatore dichiarato "static" fuori dalle funzioni e condiviso: A e B si
 *   rubano il conteggio. Ognuno deve avere il suo (variabile locale).
 * - Dimenticare k_msleep: il thread non cede mai la CPU (vedi esercizio 2).
 * - Stack troppo piccolo (es. 256): printk ne usa parecchio, il sistema si blocca.
 *
 * BONUS (facoltativo)
 * - Fai partire B con 2000 ms di ritardo (ultimo argomento di K_THREAD_DEFINE).
 * - Usa una sola funzione per entrambi i thread, passando come p1 il nome (stringa) e
 *   come p2 il periodo:  K_THREAD_DEFINE(id_a, 1024, lampeggia, "A", (void *)500, ...)
 *   e nel thread  const char *nome = p1;  int periodo = (int)(intptr_t)p2;
 *
 * Scrivi il tuo codice qui sotto.
 */

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

/* TODO: passi 1, 2 e 3 */

int main(void)
{
	/* TODO: passo 4 */

	return 0;
}
