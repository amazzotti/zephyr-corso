/*
 * statistiche.h - interfaccia del modulo "statistiche" (esercizio 8, Modulo 1).
 *
 * Questo file e' gia' completo: dice COSA offre il modulo. Il COME e' in statistiche.c,
 * che scrivi tu.
 */

#ifndef STATISTICHE_H
#define STATISTICHE_H

#include <stddef.h>
#include <stdint.h>

/* Somma degli n elementi di v. */
int32_t stat_somma(const int32_t *v, size_t n);

/* Minimo e massimo degli n elementi di v (n >= 1). */
int32_t stat_min(const int32_t *v, size_t n);
int32_t stat_max(const int32_t *v, size_t n);

/* Media moltiplicata per 10 (virgola fissa: 138 = 13.8), con la divisione intera. */
int32_t stat_media_decimi(const int32_t *v, size_t n);

/* Quante volte sono state chiamate le funzioni qui sopra, in totale.
 * "extern": la variabile e' DEFINITA in statistiche.c; qui la si dichiara soltanto. */
extern int stat_chiamate;

#endif /* STATISTICHE_H */
