/*
 * Esercizio 3 (Modulo 1) - Array e stringhe.
 *
 * Teoria: teoria/02_c_embedded.md, sezioni 1 (virgola fissa) e 4.
 *
 * OBIETTIVO
 * Lavorare con array di dimensione fissa, calcolare una media senza numeri con la
 * virgola, capire come e' fatta una stringa in C e comporre testo in un buffer senza
 * mai sforarne la dimensione.
 *
 * PASSI
 * 1. Dichiara  int voti[] = {7, 8, 6, 9, 6};  (la dimensione la deduce il compilatore).
 *    Con un ciclo for stampa "voti:" seguito da " %d" per ogni voto, poi "\n":
 *        voti: 7 8 6 9 6
 *    Usa ARRAY_SIZE(voti) come limite del ciclo, non il numero 5 scritto a mano.
 * 2. Calcola la somma e stampa "somma = 36".
 * 3. Media in VIRGOLA FISSA: calcola la media moltiplicata per 10 con soli interi
 *        int media10 = somma * 10 / n;        (n = numero di voti)
 *    e stampala come "media = 7.2" usando media10 / 10 e media10 % 10.
 * 4. Stampa "sizeof(voti) = 20, elementi = 5" (byte totali e numero di elementi).
 * 5. Dichiara  char nome[] = "Zephyr";  e stampa
 *        sizeof(nome) = 7, strlen(nome) = 6
 *    Perche' 7 e 6? (risposta nella teoria: il terminatore)
 * 6. Stampa il primo carattere e il suo codice numerico:
 *        nome[0] = 'Z' (codice 90)
 *    (%c per il carattere, %d per il codice: e' lo stesso valore!)
 * 7. Dichiara  char frase[] = "zephyr rtos";  e trasformala in maiuscolo "a mano":
 *    ciclo sui caratteri fino a '\0'; se il carattere e' tra 'a' e 'z' sottrai
 *    ('a' - 'A'). Poi stampa "maiuscolo: ZEPHYR RTOS".
 * 8. Componi in  char buf[16]  il testo "T=23 C" con
 *        int n = snprintk(buf, sizeof(buf), "T=%d C", 23);
 *    e stampa:  buf = "T=23 C" (lunghezza 6)   (la lunghezza e' n).
 *    Le virgolette dentro una stringa C si scrivono \"
 * 9. Ora in un buffer troppo piccolo,  char piccolo[8],  scrivi "Temperatura=%d" con 23.
 *    snprintk tronca. Stampa:  troncato: "Tempera" (servivano 14)
 *    dove 14 e' il valore restituito da snprintk.
 *
 * OUTPUT ATTESO
 *     voti: 7 8 6 9 6
 *     somma = 36
 *     media = 7.2
 *     sizeof(voti) = 20, elementi = 5
 *     sizeof(nome) = 7, strlen(nome) = 6
 *     nome[0] = 'Z' (codice 90)
 *     maiuscolo: ZEPHYR RTOS
 *     buf = "T=23 C" (lunghezza 6)
 *     troncato: "Tempera" (servivano 14)
 *
 * CONCETTI NUOVI
 * - Array: tipo nome[N]; indici da 0 a N-1; nessun controllo dei limiti.
 * - Ciclo for del C:  for (size_t i = 0; i < n; i++) { ... }
 * - ARRAY_SIZE(a): numero di elementi (solo su veri array, non su puntatori).
 * - Divisione intera (/) e resto (%): 360 / 50 = 7, 360 % 50 = 10.
 * - Virgola fissa: rappresentare 7.2 come 72 decimi.
 * - Stringa = array di char terminato da '\0'. strlen (da <string.h>) conta fino a li'.
 * - snprintk: scrive al massimo sizeof(buf) - 1 caratteri + '\0'; restituisce la
 *   lunghezza che AVREBBE voluto scrivere.
 *
 * COME VERIFICARE DA SOLO
 *     .\verifica.ps1 modulo1\03_array_stringhe
 *
 * ERRORI TIPICI
 * - Ciclo con i <= n invece di i < n: legge un elemento oltre la fine (valore a caso).
 * - Calcolare la media come somma / n * 10: la divisione intera avviene prima e perdi
 *   la cifra decimale (dà 70, non 72). L'ordine delle operazioni conta.
 * - Confondere 'a' (carattere) con "a" (stringa).
 * - Modificare una stringa dichiarata come  char *frase = "...";  invece di
 *   char frase[] = "...";  : la prima sta in flash (sola lettura) -> fault!
 *
 * BONUS (facoltativo)
 * - Trova il voto massimo e la sua posizione.
 * - Scrivi una funzione che conta le vocali di una stringa.
 * - Prova ad assegnare  voti[10] = 99;  : il compilatore ti avvisa? Leggi il warning
 *   (array-bounds) e poi togli la riga.
 *
 * Scrivi il tuo codice qui sotto.
 */

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys/util.h>   /* ARRAY_SIZE */
#include <string.h>            /* strlen */

int main(void)
{
	/* TODO passi 1-4: array di voti */

	/* TODO passi 5-7: stringhe */

	/* TODO passi 8-9: snprintk */

	return 0;
}
