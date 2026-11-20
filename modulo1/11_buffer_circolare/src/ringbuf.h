/*
 * ringbuf.h - buffer circolare di byte a capacita' fissa (esercizio 11, Modulo 1).
 *
 * Questo file e' gia' completo. Le funzioni le scrivi in ringbuf.c.
 * Non cambiare le firme: il Modulo 7 (modulo7/03_ztest) riusera' questo modulo per
 * scriverne i test automatici.
 */

#ifndef RINGBUF_H
#define RINGBUF_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define RB_CAPACITY 8

struct ringbuf {
	uint8_t dati[RB_CAPACITY];
	size_t testa;  /* indice dove verra' scritto il prossimo elemento (push) */
	size_t coda;   /* indice da cui verra' letto il prossimo elemento (pop)  */
	size_t conta;  /* quanti elementi ci sono adesso (0 .. RB_CAPACITY)     */
};

/* Svuota il buffer (va chiamata prima di usarlo). */
void rb_init(struct ringbuf *rb);

/* Inserisce v in fondo. Restituisce false (e non cambia nulla) se il buffer e' pieno. */
bool rb_push(struct ringbuf *rb, uint8_t v);

/* Estrae il primo elemento e lo scrive in *v. Restituisce false (e non tocca *v) se il
 * buffer e' vuoto. */
bool rb_pop(struct ringbuf *rb, uint8_t *v);

bool rb_is_empty(const struct ringbuf *rb);
bool rb_is_full(const struct ringbuf *rb);
size_t rb_count(const struct ringbuf *rb);

#endif /* RINGBUF_H */
