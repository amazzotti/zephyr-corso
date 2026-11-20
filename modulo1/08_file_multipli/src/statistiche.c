/*
 * statistiche.c - implementazione del modulo "statistiche" (esercizio 8, Modulo 1).
 *
 * Completa le funzioni dichiarate in statistiche.h. Le consegne sono in main.c.
 */

#include "statistiche.h"

/* TODO passo 2: DEFINISCI qui la variabile stat_chiamate, inizializzata a 0. */

/* TODO passo 3: una funzione PRIVATA (static) che incrementa stat_chiamate:
 *     static void registra_chiamata(void) { ... }
 * Chiamala all'inizio di ciascuna delle quattro funzioni pubbliche. */

int32_t stat_somma(const int32_t *v, size_t n)
{
	/* TODO */
	(void)v;
	(void)n;
	return 0;
}

int32_t stat_min(const int32_t *v, size_t n)
{
	/* TODO */
	(void)v;
	(void)n;
	return 0;
}

int32_t stat_max(const int32_t *v, size_t n)
{
	/* TODO */
	(void)v;
	(void)n;
	return 0;
}

int32_t stat_media_decimi(const int32_t *v, size_t n)
{
	/* TODO (puoi riusare stat_somma: ma allora quante chiamate conti?
	 * Per avere "chiamate = 4" nell'output atteso, calcola la somma qui con un ciclo.) */
	(void)v;
	(void)n;
	return 0;
}
