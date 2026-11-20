/*
 * Esercizio 2 (Modulo 1) - Operatori sui bit.
 *
 * Teoria: teoria/02_c_embedded.md, sezione 3.
 *
 * OBIETTIVO
 * Imparare le quattro operazioni che userai per tutta la vita su registri e flag:
 * accendere, spegnere, invertire e leggere un bit, piu' scrivere e leggere un "campo"
 * di piu' bit. Simuliamo un registro a 32 bit con una variabile uint32_t.
 *
 * PASSI
 * Lavora sempre sulla stessa variabile:  uint32_t reg = 0;
 * Dopo ogni operazione stampa reg in esadecimale a 8 cifre, con questo formato:
 *        printk("set bit 3: 0x%08x\n", reg);
 * 1. Stampa "iniziale: 0x00000000".
 * 2. Accendi il bit 3                          -> "set bit 3: ..."
 * 3. Accendi INSIEME i bit 0 e 5 con un solo |= -> "set bit 0,5: ..."
 * 4. Spegni il bit 3                           -> "clear bit 3: ..."
 * 5. Inverti il bit 31 (usa 1u, non 1!)        -> "toggle bit 31: ..."
 * 6. Leggi il bit 5 e il bit 4 e stampa "bit 5 = 1" e "bit 4 = 0".
 *    Il valore stampato deve essere 0 o 1, non 32: sposta il bit in fondo con >>
 *    oppure usa  (reg & BIT(5)) ? 1 : 0.
 * 7. Scrivi il valore 5 nel campo dei bit 8..10, senza toccare gli altri bit:
 *        reg = (reg & ~GENMASK(10, 8)) | (5u << 8);
 *    -> "campo 8..10 = 5: ..."
 * 8. Rileggi il campo: sposta a destra di 8 e tieni 3 bit con & 0x7
 *    -> "campo letto = 5"
 * 9. Scrivi la funzione  unsigned int conta_bit(uint32_t v)  che conta i bit a 1
 *    (ciclo: guarda il bit 0 con v & 1u, poi v >>= 1, finche' v non e' 0).
 *    -> "bit a 1 = N"
 * Prima di eseguire, calcola a mano ogni valore in binario e poi in esadecimale.
 *
 * OUTPUT ATTESO
 *     iniziale: 0x00000000
 *     set bit 3: 0x00000008
 *     set bit 0,5: 0x00000029
 *     clear bit 3: 0x00000021
 *     toggle bit 31: 0x80000021
 *     bit 5 = 1
 *     bit 4 = 0
 *     campo 8..10 = 5: 0x80000521
 *     campo letto = 5
 *     bit a 1 = 5
 *
 * CONCETTI NUOVI
 * - Operatori & | ^ ~ << >> e le forme brevi |= &= ^= >>=.
 * - Maschera: numero con a 1 solo i bit che interessano (1u << n, BIT(n), GENMASK(h, l)).
 * - Esadecimale: ogni cifra = 4 bit. 0x29 = 0010 1001 = bit 0, 3, 5.
 * - Operatore ternario: cond ? a : b (come "a if cond else b" in Python).
 *
 * COME VERIFICARE DA SOLO
 *     .\verifica.ps1 modulo1\02_bit
 *
 * ERRORI TIPICI
 * - Usare & al posto di && (o viceversa): & lavora sui bit, && sui valori logici.
 * - reg &= ~3 per spegnere il bit 3: sbagliato, spegne i bit 0 e 1 (3 = 0b11).
 *   Si scrive reg &= ~BIT(3).
 * - 1 << 31: 1 e' un int con segno, lo shift nel bit di segno e' comportamento
 *   indefinito. Usa 1u << 31 o BIT(31).
 * - Precedenza: reg & 1 << 5 == 0 non fa quello che sembra (== viene prima di &).
 *   Metti sempre le parentesi.
 *
 * BONUS (facoltativo)
 * - Scrivi le macro  SET_BIT(r, n), CLEAR_BIT(r, n), TOGGLE_BIT(r, n)  e riscrivi i passi.
 * - Zephyr ha FIELD_PREP(mask, val) e FIELD_GET(mask, reg) per i campi: prova a usarle
 *   al posto dello shift a mano (cerca la definizione in zephyr/include/zephyr/sys/).
 * - Stampa reg in binario, 32 cifre, con un ciclo.
 *
 * Scrivi il tuo codice qui sotto.
 */

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys/util.h>   /* BIT, GENMASK */

/* TODO passo 9: unsigned int conta_bit(uint32_t v) */

int main(void)
{
	uint32_t reg = 0;

	printk("iniziale: 0x%08x\n", reg);

	/* TODO passi 2-9 */

	return 0;
}
