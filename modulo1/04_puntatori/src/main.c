/*
 * Esercizio 4 (Modulo 1) - Puntatori.
 *
 * Teoria: teoria/02_c_embedded.md, sezione 5.
 *
 * OBIETTIVO
 * Capire i puntatori, il concetto piu' importante del C (e quello che in Python non si
 * vede mai): indirizzi, dereferenza, funzioni che modificano variabili del chiamante,
 * puntatori e array, NULL.
 *
 * PASSI
 * 1. int a = 10;  int *p = &a;
 *    Stampa "a = 10, *p = 10" (il secondo valore leggilo tramite p).
 * 2. Scrivi 20 in a PASSANDO DA p (*p = 20) e stampa "dopo *p = 20: a = 20".
 * 3. Stampa "p punta ad a? SI" se p == &a, altrimenti "NO".
 *    Bonus del passo: stampa anche l'indirizzo con %p. Che numero e'? (RAM: 0x2000....)
 * 4. Scrivi  void swap(int *x, int *y)  che scambia i due valori. Con x = 3, y = 7,
 *    dopo swap(&x, &y) stampa "swap: x = 7, y = 3".
 * 5. Scrivi
 *        void min_max(const int *v, size_t n, int *min, int *max)
 *    che trova minimo e massimo di un array e li "restituisce" tramite i puntatori.
 *    Su  int dati[] = {4, -2, 9, 0, 7};  stampa "min = -2, max = 9".
 * 6. Aritmetica dei puntatori:  int v[4] = {10, 20, 30, 40};  int *q = v;
 *    - stampa "*(q + 2) = 30"
 *    - stampa "(q + 1) - q in byte = 4": converti i due puntatori in numeri con
 *      (uintptr_t) e sottraili. Perche' 4 e non 1?
 * 7. Somma gli elementi di v SENZA usare indici: un puntatore che parte da v e avanza
 *    con ++ finche' e' minore di v + 4. Stampa "somma con puntatore = 100".
 * 8. Scrivi  int *trova(int *v, size_t n, int valore)  che restituisce il puntatore
 *    all'elemento uguale a valore, oppure NULL se non c'e'.
 *    - trova(v, 4, 30): stampa "trovato 30 all'indice 2" (indice = risultato - v)
 *    - trova(v, 4, 99): controlla il NULL e stampa "99 non trovato"
 *
 * OUTPUT ATTESO
 *     a = 10, *p = 10
 *     dopo *p = 20: a = 20
 *     p punta ad a? SI
 *     swap: x = 7, y = 3
 *     min = -2, max = 9
 *     *(q + 2) = 30
 *     (q + 1) - q in byte = 4
 *     somma con puntatore = 100
 *     trovato 30 all'indice 2
 *     99 non trovato
 *
 * CONCETTI NUOVI
 * - &x: indirizzo di x.   int *p: puntatore a int.   *p: valore puntato.
 * - Passaggio per valore (copia) e "per riferimento" passando l'indirizzo.
 * - const int *v: la funzione promette di non modificare gli elementi.
 * - p + k avanza di k ELEMENTI (k * sizeof(*p) byte).  p[k] e' lo stesso di *(p + k).
 * - uintptr_t: intero grande abbastanza da contenere un indirizzo.
 * - NULL: puntatore che non punta a niente; va controllato prima di usarlo.
 *
 * COME VERIFICARE DA SOLO
 *     .\verifica.ps1 modulo1\04_puntatori
 *
 * ERRORI TIPICI
 * - swap(x, y) invece di swap(&x, &y): il compilatore avvisa ("makes pointer from
 *   integer") e a runtime scrive a indirizzi 3 e 7 -> fault.
 * - Dentro swap scrivere  x = y;  (scambi i puntatori locali, non i valori): serve *x.
 * - Usare un puntatore non inizializzato (int *p; *p = 5;): scrive in un punto a caso.
 * - Dereferenziare il risultato di trova() senza controllare NULL.
 * - Restituire l'indirizzo di una variabile locale: dopo il return quella memoria
 *   non e' piu' tua.
 *
 * BONUS (facoltativo)
 * - Prova a dereferenziare NULL (int *z = NULL; printk("%d\n", *z);). Su questo chip
 *   l'indirizzo 0 e' l'inizio della flash, quindi la LETTURA potrebbe non fallire:
 *   stampa il primo word della tabella dei vettori (valore iniziale dello stack, 0x2000....).
 *   Sorpresa embedded! Poi togli la riga.
 * - Scrivi  void inverti(int *v, size_t n)  che rovescia un array sul posto usando
 *   due puntatori (uno dall'inizio, uno dalla fine) e swap.
 *
 * Scrivi il tuo codice qui sotto.
 */

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <stdint.h>
#include <stddef.h>

/* TODO passo 4: void swap(int *x, int *y) */

/* TODO passo 5: void min_max(const int *v, size_t n, int *min, int *max) */

/* TODO passo 8: int *trova(int *v, size_t n, int valore) */

int main(void)
{
	/* TODO passi 1-8 */

	return 0;
}
