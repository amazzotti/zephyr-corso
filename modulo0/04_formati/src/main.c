/*
 * Esercizio 4 (Modulo 0) - printk e i formati
 *
 * Teoria: teoria/01_setup_toolchain.md, sezione 8.
 *
 * OBIETTIVO
 * Imparare a stampare numeri, testo e indirizzi con printk, come facevi con le
 * f-string in Python, e stampare due informazioni dal kernel: la versione di Zephyr e
 * il tempo passato dall'accensione.
 *
 * PASSI
 * 1. Nello scheletro ci sono gia' alcune variabili. Stampa ciascuna con il formato
 *    giusto, una riga per variabile, ESATTAMENTE cosi':
 *        Intero: -42
 *        Senza segno: 3000000000
 *        Esadecimale: 0x2a
 *        Esadecimale 8 cifre: 0x0000002a
 *        Carattere: Z
 *        Testo: microcontrollore
 *    Formati: %d (intero con segno), %u (senza segno), %x (esadecimale minuscolo),
 *    %08x (esadecimale su 8 cifre con zeri davanti), %c (un carattere), %s (stringa).
 *    "0x" non lo aggiunge il formato: scrivilo tu nel testo.
 * 2. Stampa piu' valori nella stessa riga:
 *        Somma: 7 + 5 = 12
 *    usando le variabili a e b e calcolando la somma dentro printk.
 * 3. Stampa la versione del kernel. Zephyr la fornisce come numero a 32 bit:
 *        uint32_t v = sys_kernel_version_get();
 *    e le macro SYS_KERNEL_VER_MAJOR(v), SYS_KERNEL_VER_MINOR(v),
 *    SYS_KERNEL_VER_PATCHLEVEL(v) ne estraggono le tre parti. Riga attesa:
 *        Versione kernel: 4.x.y
 *    (con i numeri veri della tua versione).
 * 4. Stampa il tempo dall'accensione in millisecondi:
 *        int64_t t = k_uptime_get();
 *        printk("Uptime: %u ms\n", (uint32_t)t);
 *    Il cast (uint32_t) serve perche' %u vuole un numero a 32 bit e t e' a 64.
 * 5. Stampa l'indirizzo in memoria della variabile "testo" con %p:
 *        Indirizzo di testo: 0x...
 *    E' in flash (0x08......) o in RAM (0x2000....)? Ci torniamo nel Modulo 1.
 *
 * OUTPUT ATTESO (le ultime righe dipendono dalla versione e dal momento)
 *     Intero: -42
 *     Senza segno: 3000000000
 *     Esadecimale: 0x2a
 *     Esadecimale 8 cifre: 0x0000002a
 *     Carattere: Z
 *     Testo: microcontrollore
 *     Somma: 7 + 5 = 12
 *     Versione kernel: 4.2.0
 *     Uptime: 3 ms
 *     Indirizzo di testo: 0x80.....
 * (%p non mette gli zeri davanti: 0x8003b0a e' l'indirizzo 0x08003B0A)
 *
 * CONCETTI NUOVI
 * - Specificatori di formato: dicono a printk COME interpretare il valore passato.
 *   Se formato e tipo non corrispondono, il compilatore di solito avvisa (warning)
 *   e il risultato stampato e' sbagliato.
 * - Tipi: int (con segno), unsigned int / uint32_t (senza segno), char (un carattere),
 *   const char * (stringa). Li vediamo bene nel Modulo 1.
 * - k_uptime_get(): millisecondi dall'accensione, dal timer di sistema.
 * - printk NON stampa numeri con la virgola (float) se non lo abiliti: in embedded si
 *   preferiscono spesso numeri interi (es. millesimi di grado invece di gradi).
 *
 * COME VERIFICARE DA SOLO
 *     .\verifica.ps1 modulo0\04_formati
 *
 * ERRORI TIPICI
 * - Usare %d per 3000000000: non sta in un int con segno, esce un numero negativo.
 * - Dimenticare uno dei valori dopo il formato: printk stampa "spazzatura" al suo posto.
 * - %x stampa minuscolo (2a), %X maiuscolo (2A): la verifica vuole minuscolo.
 * - Passare k_uptime_get() direttamente a %u senza cast: il valore e' a 64 bit.
 *
 * BONUS (facoltativo)
 * - Stampa -42 con %u e con %x: cosa esce e perche'? (complemento a due, Modulo 1)
 * - Allinea in colonna con le larghezze: printk("[%5d]\n", 42) e printk("[%-5d]\n", 42).
 * - Stampa CONFIG_SYS_CLOCK_TICKS_PER_SEC: quante volte al secondo "batte" il timer
 *   di sistema?
 *
 * Scrivi il tuo codice qui sotto.
 */

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/version.h>

int main(void)
{
	int intero = -42;
	unsigned int grande = 3000000000U;
	int risposta = 42;
	char lettera = 'Z';
	const char *testo = "microcontrollore";
	int a = 7;
	int b = 5;

	/* TODO: passi 1-5. */

	/* Evita i warning "variabile non usata" finche' non le usi tu: poi cancella. */
	(void)intero; (void)grande; (void)risposta; (void)lettera; (void)testo;
	(void)a; (void)b;

	return 0;
}
