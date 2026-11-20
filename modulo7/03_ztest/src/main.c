/*
 * Esercizio 3 (Modulo 7) - Test con ztest
 *
 * Teoria: teoria/08_strumenti_debug_test.md, sezione 3.
 *
 * OBIETTIVO
 * Scrivere i test automatici del buffer circolare del Modulo 1
 * (modulo1/11_buffer_circolare/src/ringbuf.c). Nel Modulo 1 lo provavi "a mano" con
 * printk; ora ogni proprieta' diventa un test che passa o fallisce, e si rilancia
 * tutto con un comando dopo ogni modifica.
 *
 * Il buffer (ringbuf.h) offre: RB_CAPACITY (8), rb_init, rb_push(rb, byte) -> bool,
 * rb_pop(rb, &byte) -> bool, rb_is_empty, rb_is_full, rb_count. Il tipo della struct
 * e' quello che hai definito in ringbuf.h (es. struct ringbuf).
 *
 * PASSI
 * 1. Esegui lo scheletro: .\esegui.ps1 modulo7\03_ztest
 *    C'e' un solo test (test_capacita): deve passare. Leggi il riepilogo finale.
 *    Se la compilazione fallisce perche' manca ringbuf.c, finisci prima l'esercizio
 *    modulo1/11_buffer_circolare.
 * 2. Scrivi almeno questi test (uno per ZTEST, nomi esatti):
 *        test_vuoto_iniziale   dopo rb_init: vuoto, non pieno, count 0
 *        test_push_pop_uno     push di 42, pop -> true e il valore e' 42, poi vuoto
 *        test_ordine_fifo      push 1,2,3 -> pop restituisce 1,2,3 in quest'ordine
 *        test_pieno            8 push riusciti, il 9o restituisce false, count == 8
 *        test_pop_vuoto        pop da buffer vuoto -> false, e il byte NON viene toccato
 *        test_giro             riempi, svuota a meta', riempi di nuovo: l'ordine e' giusto
 *                              anche quando gli indici "girano" oltre la fine dell'array
 *        test_count            count giusto dopo una sequenza mista di push e pop
 *    Ogni test dichiara il SUO buffer (variabile locale) e chiama rb_init: i test devono
 *    essere indipendenti tra loro.
 * 3. Rompi apposta ringbuf.c (es. in rb_is_full scrivi RB_CAPACITY - 1) e rilancia:
 *    quali test falliscono? Il messaggio ti porta al problema? Poi rimetti a posto.
 *
 * OUTPUT ATTESO (alla fine)
 *     SUITE PASS - 100.00% [ringbuf]: pass = 8, fail = 0, skip = 0, total = 8 ...
 *     PROJECT EXECUTION SUCCESSFUL
 *
 * CONCETTI NUOVI
 * - Test automatico: codice che controlla altro codice. Il riepilogo dice in un colpo
 *   se tutto e' ancora giusto (test di regressione).
 * - ZTEST(suite, nome), ZTEST_SUITE(...), zassert_true / zassert_equal / ...
 * - Nessun main: lo fornisce ztest (CONFIG_ZTEST=y).
 * - Riuso dei sorgenti tra applicazioni: guarda CMakeLists.txt, che compila ringbuf.c
 *   direttamente dalla cartella del Modulo 1.
 * - Casi limite: buffer vuoto, pieno, indici che tornano a zero. I bug stanno li'.
 *
 * COME VERIFICARE DA SOLO
 *     .\verifica.ps1 modulo7\03_ztest
 * controlla che almeno 7 test passino e che nessuno fallisca.
 *
 * ERRORI TIPICI
 * - Un buffer globale condiviso tra i test: un test lascia dati che fanno fallire il
 *   successivo, e il risultato dipende dall'ordine.
 * - zassert_equal con il messaggio mancante: il terzo argomento e' obbligatorio
 *   (puo' essere "" ma e' meglio spiegare cosa ci si aspettava).
 * - Scrivere un main(): va in conflitto con quello di ztest.
 * - Confrontare uint8_t con valori > 255 (es. push di 300): il valore viene troncato.
 *
 * BONUS (facoltativo)
 * - Usa la funzione "before" di ZTEST_SUITE per inizializzare un buffer di test prima di
 *   ogni test (e confronta con il buffer locale).
 * - Test "stress": 1000 push/pop alternati con valori i % 256, controllando ogni valore.
 * - Nel Modulo 1 avevi scritto test "a mano": quali casi non avevi coperto?
 *
 * Scrivi il tuo codice qui sotto.
 */

#include <zephyr/ztest.h>
#include "ringbuf.h"

ZTEST(ringbuf, test_capacita)
{
	zassert_equal(RB_CAPACITY, 8, "la capacita' del buffer deve essere 8");
}

/* TODO: i test del passo 2. */

ZTEST_SUITE(ringbuf, NULL, NULL, NULL, NULL, NULL);
