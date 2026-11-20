/*
 * Esercizio 1 (Modulo 1) - Tipi e dimensioni.
 *
 * Teoria: teoria/02_c_embedded.md, sezioni 1 e 2.
 *
 * OBIETTIVO
 * Scoprire quanti byte occupano i tipi del C sul Cortex-M4 e vedere con i propri occhi
 * cosa succede quando un numero non ci sta nel suo tipo (overflow), e perche' mescolare
 * numeri con e senza segno e' pericoloso. In Python niente di tutto questo esiste: gli
 * interi crescono quanto serve.
 *
 * PASSI
 * 1. Stampa la dimensione dei tipi con sizeof, una riga per tipo, in questo formato
 *    esatto (sizeof restituisce un size_t: convertilo con (unsigned int) e usa %u):
 *        printk("sizeof(char) = %u\n", (unsigned int)sizeof(char));
 *    Tipi, in quest'ordine: char, short, int, long, long long, uint8_t, uint16_t,
 *    uint32_t, uint64_t, void *  (per l'ultimo il testo e' "sizeof(void *)").
 * 2. Overflow senza segno:
 *        uint8_t u = 250;
 *        u = u + 10;
 *    Stampa "uint8_t: 250 + 10 = " seguito dal valore di u (%u).
 * 3. Overflow con segno: int8_t s = 127; s = s + 1;
 *    Stampa "int8_t: 127 + 1 = " seguito da s (%d).
 *    (Il risultato che vedrai e' quello di GCC su ARM; lo standard C non lo garantisce:
 *    leggi la teoria.)
 * 4. Sottrazione senza segno: uint32_t a = 3, b = 5;
 *    Stampa "uint32_t: 3 - 5 = " seguito da a - b (%u).
 * 5. Confronto pericoloso: int x = -1; unsigned int y = 1;
 *    Stampa "-1 < 1u ? SI" se x < y, altrimenti "-1 < 1u ? NO".
 *    Guarda anche i warning del compilatore su quella riga.
 *
 * OUTPUT ATTESO (dopo il banner di avvio)
 *     sizeof(char) = 1
 *     sizeof(short) = 2
 *     sizeof(int) = 4
 *     sizeof(long) = 4
 *     sizeof(long long) = 8
 *     sizeof(uint8_t) = 1
 *     sizeof(uint16_t) = 2
 *     sizeof(uint32_t) = 4
 *     sizeof(uint64_t) = 8
 *     sizeof(void *) = 4
 *     uint8_t: 250 + 10 = 4
 *     int8_t: 127 + 1 = -128
 *     uint32_t: 3 - 5 = 4294967294
 *     -1 < 1u ? NO
 *
 * CONCETTI NUOVI
 * - Dichiarazione di variabile con tipo: uint8_t u = 250;
 * - sizeof: dimensione in byte di un tipo o di una variabile, calcolata in compilazione.
 * - Tipi a larghezza fissa (<stdint.h>, gia' incluso da kernel.h): intN_t, uintN_t.
 * - Overflow: i tipi senza segno "girano" (modulo 2^N), come un contachilometri.
 * - Conversione implicita: in un confronto int / unsigned, l'int diventa unsigned
 *   e -1 diventa 4294967295.
 * - Cast: (unsigned int)valore converte esplicitamente un valore in un altro tipo.
 *
 * COME VERIFICARE DA SOLO
 *     .\verifica.ps1 modulo1\01_tipi
 * Prima prova a PREVEDERE ogni riga su carta, poi esegui e confronta.
 *
 * ERRORI TIPICI
 * - Usare %d per un valore unsigned grande: 4294967294 viene stampato come -2.
 * - Stampare sizeof con %d senza cast: warning sul formato (size_t non e' int).
 * - Scrivere u + 10 dentro printk senza riassegnare a u: l'espressione viene calcolata
 *   come int (promozione) e stampa 260! Il "taglio" a 8 bit avviene solo quando il
 *   risultato viene salvato in un uint8_t.
 *
 * BONUS (facoltativo)
 * - Stampa UINT8_MAX, INT16_MIN e UINT32_MAX (macro di <stdint.h>).
 * - Stampa (uint8_t)300 e (int8_t)200: prevedi il risultato prima di eseguire.
 * - Quale tipo useresti per: una temperatura in decimi di grado (-400 .. 1250)? Il numero
 *   di millisecondi dall'accensione? Il valore di un registro a 32 bit? Scrivilo qui.
 *
 * Scrivi il tuo codice qui sotto.
 */

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

int main(void)
{
	/* TODO passo 1: dimensioni dei tipi */

	/* TODO passo 2: overflow di uint8_t */

	/* TODO passo 3: overflow di int8_t */

	/* TODO passo 4: sottrazione senza segno */

	/* TODO passo 5: confronto con e senza segno */

	return 0;
}
