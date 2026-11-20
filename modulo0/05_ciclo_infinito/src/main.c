/*
 * Esercizio 5 (Modulo 0) - Un firmware non finisce mai
 *
 * Teoria: teoria/01_setup_toolchain.md, sezioni 2, 7 e 9 (tempo virtuale).
 *
 * OBIETTIVO
 * Scrivere il classico "super-loop": un ciclo infinito che ogni secondo stampa un
 * contatore e il tempo dall'accensione. E' la struttura di base di quasi ogni firmware.
 *
 * PASSI
 * 1. Stampa una volta, all'inizio:
 *        Avvio del ciclo
 * 2. Crea una variabile contatore che parte da 0:
 *        uint32_t conteggio = 0;
 * 3. Scrivi un ciclo infinito. In C:
 *        while (1) {
 *            ...
 *        }
 *    (equivale a "while True:" di Python).
 * 4. Dentro il ciclo:
 *    - stampa  "Conteggio: N - uptime: T ms"  con N = conteggio e T = k_uptime_get()
 *      convertito a uint32_t (come nell'esercizio 4);
 *    - aumenta il contatore:  conteggio++;   (come conteggio += 1 in Python)
 *    - aspetta un secondo:    k_msleep(1000);
 * 5. Esegui con esegui.ps1 e guarda la UART per una decina di secondi. Poi nel monitor
 *    prova  pause  e  start: il conteggio si ferma e riprende da dove era.
 *
 * OUTPUT ATTESO
 *     Avvio del ciclo
 *     Conteggio: 0 - uptime: 2 ms
 *     Conteggio: 1 - uptime: 1003 ms
 *     Conteggio: 2 - uptime: 2004 ms
 *     ...
 * (i millisecondi possono variare di poco: tra due righe passano circa 1000 ms)
 *
 * CONCETTI NUOVI
 * - Super-loop: in un microcontrollore non c'e' nessuno a cui "tornare" alla fine del
 *   programma. Il firmware gira finche' c'e' corrente.
 * - k_msleep(ms): il thread corrente DORME per quel tempo. Non e' un ciclo che conta a
 *   vuoto: il kernel mette la CPU in risparmio energetico finche' non scade il timer.
 *   Questa differenza (dormire contro aspettare in modo attivo) e' la chiave del
 *   Modulo 3.
 * - Tempo virtuale: in Renode un secondo della scheda non e' sempre un secondo del tuo
 *   orologio. Contano i tempi stampati dal firmware, non quelli che misuri tu.
 *
 * COME VERIFICARE DA SOLO
 *     .\verifica.ps1 modulo0\05_ciclo_infinito
 * Controlla le prime righe del conteggio.
 *
 * ERRORI TIPICI
 * - Mettere "conteggio = 0" DENTRO il ciclo: stampa sempre 0.
 * - Dimenticare k_msleep: il ciclo stampa a raffica e satura la UART.
 * - Scrivere k_msleep(1): e' 1 ms, non 1 secondo.
 *
 * BONUS (facoltativo)
 * - Stampa ogni 5 conteggi anche "--- 5 secondi ---" (operatore %, come in Python).
 * - Calcola e stampa la differenza tra l'uptime di questa riga e quello della precedente:
 *   e' sempre esattamente 1000? Perche' potrebbe non esserlo? (Modulo 5, jitter)
 * - Fai finire il ciclo dopo 10 conteggi con "break" e stampa "Fine": cosa fa poi la
 *   CPU? (guarda cpu PC nel monitor, come nell'esercizio 3)
 *
 * Scrivi il tuo codice qui sotto.
 */

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

int main(void)
{
	/* Scrivi qui il tuo codice. */

	return 0;
}
