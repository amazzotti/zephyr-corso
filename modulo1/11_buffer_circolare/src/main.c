/*
 * Esercizio 11 (Modulo 1) - Riepilogo: il buffer circolare.
 *
 * Teoria: teoria/02_c_embedded.md, sezioni 5, 6, 9 e 12.
 *
 * OBIETTIVO
 * Mettere insieme struct, puntatori, array, file multipli e bool per costruire una
 * struttura dati che userai davvero: il buffer circolare (coda FIFO di capacita' fissa).
 * E scrivere i primi TEST: codice che controlla altro codice.
 *
 * PASSI
 * 1. Leggi ringbuf.h (gia' completo) e la sezione 12 della teoria. Disegna su carta un
 *    buffer da 8 caselle e simula a mano: push A, B, C; pop; push D... seguendo testa,
 *    coda e conta.
 * 2. Completa ringbuf.c. Il "giro" degli indici si fa con  i = (i + 1) % RB_CAPACITY;
 * 3. In main, la funzione controlla() e' gia' pronta: stampa [OK] o [ERRORE] e conta.
 *    Scrivi questi 10 test, IN QUEST'ORDINE e con queste descrizioni esatte, usando
 *    una sola  struct ringbuf rb;  e  uint8_t v;  :
 *     1 "vuoto dopo init"        dopo rb_init: rb_is_empty e' vero
 *     2 "conta 0 dopo init"      rb_count vale 0
 *     3 "pop da vuoto fallisce"  rb_pop restituisce false
 *     4 "push di 42"             rb_push(&rb, 42) vero E rb_count vale 1
 *     5 "pop restituisce 42"     rb_pop vero E v == 42 E il buffer e' di nuovo vuoto
 *     6 "riempito con 8"         push di 0, 1, ..., 7: tutti veri, E poi rb_is_full
 *     7 "push su pieno fallisce" rb_push(&rb, 99) falso E rb_count ancora 8
 *     8 "ordine FIFO"            8 pop: escono 0, 1, ..., 7 in ordine
 *     9 "giro degli indici"      rb_init; push 10..15 (6 elementi); pop 4 volte
 *                                (devono uscire 10..13); push 16..21 (6 elementi):
 *                                ora e' pieno; 8 pop: devono uscire 14, 15, ..., 21
 *    10 "vuoto alla fine"        rb_is_empty vero E rb_pop falso
 *    Per i test fatti di molti passi (6, 8, 9) usa una variabile  bool ok = true;  e
 *    mettila a false appena un controllo fallisce; poi  controlla(ok, "...").
 * 4. Esegui: con ringbuf.c ancora vuoto, quasi tutti i test sono [ERRORE]. Completa le
 *    funzioni finche' non sono tutti [OK].
 *
 * OUTPUT ATTESO
 *     [OK] vuoto dopo init
 *     [OK] conta 0 dopo init
 *     [OK] pop da vuoto fallisce
 *     [OK] push di 42
 *     [OK] pop restituisce 42
 *     [OK] riempito con 8
 *     [OK] push su pieno fallisce
 *     [OK] ordine FIFO
 *     [OK] giro degli indici
 *     [OK] vuoto alla fine
 *     RISULTATO: 10/10 test superati
 *
 * CONCETTI NUOVI
 * - bool, true, false (<stdbool.h>).
 * - Funzione che puo' fallire: restituisce bool e scrive il risultato tramite puntatore.
 * - Aritmetica modulare (%) per far girare un indice.
 * - Test: preparare uno stato, chiamare la funzione, controllare il risultato. Il test 9
 *   e' quello che trova i bug veri: quando testa e coda "girano" oltre la fine.
 *
 * COME VERIFICARE DA SOLO
 *     .\verifica.ps1 modulo1\11_buffer_circolare
 *
 * ERRORI TIPICI
 * - Avanzare l'indice con i++ senza il %: dopo 8 push si scrive fuori dall'array
 *   (dati[8]) e si corrompono testa/coda, che stanno subito dopo in memoria!
 * - Distinguere pieno/vuoto con testa == coda: e' vero in ENTRAMBI i casi. Per questo
 *   c'e' il campo conta.
 * - In rb_pop modificare *v anche quando il buffer e' vuoto.
 * - Dimenticare rb_init: i campi della struct locale contengono valori casuali
 *   (le variabili locali non sono azzerate, a differenza delle globali in .bss).
 *
 * BONUS (facoltativo)
 * - Aggiungi  bool rb_peek(const struct ringbuf *rb, uint8_t *v)  (legge senza togliere)
 *   e il suo test.
 * - Se RB_CAPACITY fosse una potenza di 2, al posto di % RB_CAPACITY si puo' usare
 *   & (RB_CAPACITY - 1). Perche' funziona? Perche' e' piu' veloce su un microcontrollore?
 * - Zephyr ha un buffer circolare pronto: <zephyr/sys/ring_buffer.h>. Leggi l'header e
 *   confronta le funzioni con le tue.
 *
 * Scrivi il tuo codice qui sotto.
 */

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <stdbool.h>
#include "ringbuf.h"

static int test_totali;
static int test_superati;

/* Registra l'esito di un test e lo stampa. Gia' pronta: usala. */
static void controlla(bool ok, const char *descrizione)
{
	test_totali++;
	if (ok) {
		test_superati++;
		printk("[OK] %s\n", descrizione);
	} else {
		printk("[ERRORE] %s\n", descrizione);
	}
}

int main(void)
{
	/* TODO passo 3: i 10 test */

	printk("RISULTATO: %d/%d test superati\n", test_superati, test_totali);
	return 0;
}
