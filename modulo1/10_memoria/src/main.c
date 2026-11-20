/*
 * Esercizio 10 (Modulo 1) - Dove vivono codice e variabili.
 *
 * Teoria: teoria/02_c_embedded.md, sezione 10.
 *
 * OBIETTIVO
 * Scoprire DOVE finiscono in memoria funzioni, costanti, variabili globali, static e
 * locali: in flash o in RAM, e in quale sezione (.text, .rodata, .data, .bss, stack).
 * Su un microcontrollore con 256 KB di RAM e' una domanda che ti farai sempre.
 *
 * PASSI
 * 1. Le variabili da studiare sono gia' dichiarate qui sotto (globali) e in main
 *    (statica_locale e locale). Non cambiarle.
 * 2. Scrivi  const char *zona(uint32_t indirizzo)  che restituisce:
 *        "FLASH" se indirizzo < 0x00100000
 *        "RAM"   se 0x20000000 <= indirizzo < 0x20040000
 *        "?"     altrimenti
 * 3. Scrivi  void mostra(const char *nome, const void *p)  che stampa una riga con
 *    questo formato ESATTO:
 *        printk("%-14s 0x%08x %s\n", nome, indirizzo, zona(indirizzo));
 *    dove indirizzo = (uint32_t)(uintptr_t)p.
 * 4. In main chiama mostra() per, in quest'ordine:
 *        "main"           (uintptr_t)main  -> per una funzione usa il nome senza ()
 *        "stringa"        la stringa letterale "ciao" (passa direttamente "ciao")
 *        "costante"       &costante
 *        "globale_init"   &globale_init
 *        "globale_zero"   &globale_zero
 *        "statica_init"   &statica_init
 *        "statica_locale" &statica_locale
 *        "locale"         &locale
 *    Per main il compilatore puo' lamentarsi di convertire un puntatore a funzione in
 *    const void *: passa  (const void *)(uintptr_t)main.
 * 5. Stampa "globale_zero vale %u": vale 0 anche se non l'hai inizializzata. Chi l'ha
 *    azzerata? (teoria: .bss)
 * 6. Apri build/zephyr/zephyr.map e cerca  globale_init  e  globale_zero: in che
 *    sezione sono (.data? .bss?) e gli indirizzi coincidono con quelli stampati?
 * 7. Rispondi qui sotto (in questo commento):
 *    a) Perche' "costante" e' in flash e "globale_init" in RAM?
 *    b) globale_init vale 42 all'avvio: dove era scritto il 42 prima dell'accensione?
 *    c) Perche' "locale" ha un indirizzo cosi' diverso dalle globali?
 *    Risposte:
 *
 * OUTPUT ATTESO (gli indirizzi esatti dipendono dalla compilazione; conta la zona)
 *     main           0x000..... FLASH
 *     stringa        0x000..... FLASH
 *     costante       0x000..... FLASH
 *     globale_init   0x2000.... RAM
 *     globale_zero   0x2000.... RAM
 *     statica_init   0x2000.... RAM
 *     statica_locale 0x2000.... RAM
 *     locale         0x2000.... RAM
 *     globale_zero vale 0
 * (tra indirizzo e zona c'e' un solo spazio; i puntini indicano cifre qualsiasi)
 *
 * CONCETTI NUOVI
 * - Mappa di memoria del chip: flash da 0x00000000, RAM da 0x20000000.
 * - Sezioni: .text (codice), .rodata (costanti), .data (globali inizializzate),
 *   .bss (globali a zero), stack (locali).
 * - Il codice di avvio copia .data da flash a RAM e azzera .bss prima di main.
 * - uintptr_t: intero della dimensione di un indirizzo; cast puntatore <-> numero.
 * - Il file .map del linker.
 *
 * COME VERIFICARE DA SOLO
 *     .\verifica.ps1 modulo1\10_memoria
 *
 * ERRORI TIPICI
 * - Stampare un indirizzo con %d: i valori 0x2000.... diventano numeri enormi o negativi.
 * - Confrontare con <= 0x00100000 invece di <: l'indirizzo 0x00100000 e' gia' fuori dalla
 *   flash (1 MB = 0x100000 byte, da 0 a 0xFFFFF).
 * - Stupirsi che l'indirizzo di main sia DISPARI: sui Cortex-M il bit 0 di un indirizzo
 *   di funzione indica le istruzioni "Thumb". La funzione inizia all'indirizzo pari.
 *
 * BONUS (facoltativo)
 * - Aggiungi un array globale  uint8_t grande[10000];  e confronta la tabella
 *   "Memory region" di west build prima e dopo: quale riga cresce? E con
 *   const uint8_t grande[10000] = {1};  ?
 * - Stampa l'indirizzo di una variabile locale dentro una funzione chiamata da main e
 *   dentro una chiamata da quella: lo stack cresce verso indirizzi piu' alti o piu' bassi?
 *
 * Scrivi il tuo codice qui sotto.
 */

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <stdint.h>

/* Le variabili da studiare: non modificarle. */
const uint32_t costante = 0x12345678;    /* const globale            */
uint32_t globale_init = 42;              /* globale inizializzata    */
uint32_t globale_zero;                   /* globale non inizializzata */
static uint32_t statica_init = 7;        /* static di file           */

/* TODO passo 2: const char *zona(uint32_t indirizzo) */

/* TODO passo 3: void mostra(const char *nome, const void *p) */

int main(void)
{
	static uint32_t statica_locale;      /* static dentro una funzione */
	uint32_t locale = 5;                 /* variabile locale           */

	/* TODO passi 4-5 */

	/* Queste righe evitano i warning "variabile non usata" finche' non scrivi il codice:
	 * puoi toglierle quando le usi. */
	(void)statica_locale;
	(void)locale;
	(void)statica_init;

	return 0;
}
