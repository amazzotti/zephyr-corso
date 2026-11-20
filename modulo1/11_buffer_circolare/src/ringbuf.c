/*
 * ringbuf.c - implementazione del buffer circolare (esercizio 11, Modulo 1).
 *
 * Completa le funzioni: la consegna e' in main.c, la teoria in
 * teoria/02_c_embedded.md, sezione 12.
 */

#include "ringbuf.h"

void rb_init(struct ringbuf *rb)
{
	/* TODO: azzera testa, coda e conta */
	(void)rb;
}

bool rb_push(struct ringbuf *rb, uint8_t v)
{
	/* TODO: se pieno -> false. Altrimenti scrivi in dati[testa], avanza testa
	 * in modo circolare (% RB_CAPACITY), incrementa conta, restituisci true. */
	(void)rb;
	(void)v;
	return false;
}

bool rb_pop(struct ringbuf *rb, uint8_t *v)
{
	/* TODO: se vuoto -> false. Altrimenti leggi dati[coda] in *v, avanza coda,
	 * decrementa conta, restituisci true. */
	(void)rb;
	(void)v;
	return false;
}

bool rb_is_empty(const struct ringbuf *rb)
{
	/* TODO */
	(void)rb;
	return false;
}

bool rb_is_full(const struct ringbuf *rb)
{
	/* TODO */
	(void)rb;
	return false;
}

size_t rb_count(const struct ringbuf *rb)
{
	/* TODO */
	(void)rb;
	return 0;
}
