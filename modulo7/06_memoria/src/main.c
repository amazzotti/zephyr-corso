/*
 * Esercizio 6 (Modulo 7) - Memoria
 *
 * Teoria: teoria/08_strumenti_debug_test.md, sezione 6.
 * Ripasso: modulo1/10_memoria (sezioni .text, .rodata, .data, .bss, stack).
 *
 * OBIETTIVO
 * Misurare quanta flash e quanta RAM usa un firmware, capire CHI le usa, e ridurre gli
 * stack dei thread in base a misure reali invece che "a occhio".
 *
 * Il programma qui sotto e' gia' scritto: e' l'oggetto delle misure. Tu modifichi solo
 * le dimensioni (STACK_LAVORATORE, STACK_PICCOLO, DIM_BUFFER) e rispondi alle domande.
 *
 * PASSI
 * 1. Compila: .\esegui.ps1 modulo7\06_memoria
 *    Annota la tabella "Memory region" (FLASH e RAM usate). (Domanda 1)
 * 2. Report dettagliati, dalla cartella del repo e con l'ambiente attivo:
 *        west build -d modulo7\06_memoria\build -t ram_report
 *        west build -d modulo7\06_memoria\build -t rom_report
 *    Nel ram_report cerca buffer_ram e gli stack dei thread (lavoratore, piccolo).
 *    Nel rom_report cerca tabella_flash. (Domande 2 e 3)
 * 3. In Renode, ogni 5 secondi il thread analyzer stampa per ogni thread:
 *        lavoratore : STACK: unused 3624 usage 472 / 4096 (11 %); CPU: ...
 *    Lascia girare almeno 15 secondi (il caso peggiore deve essere avvenuto) e annota
 *    "usage" di lavoratore e piccolo. (Domanda 4)
 * 4. Ridimensiona: per ciascun thread nuovo stack = usage misurato + 50%, arrotondato al
 *    multiplo di 256 superiore (minimo 512). Cambia STACK_LAVORATORE e STACK_PICCOLO,
 *    ricompila, confronta la RAM usata con il passo 1.
 * 5. Esperimento: metti STACK_LAVORATORE a 256 e guarda cosa succede (e' un errore
 *    voluto: poi rimetti il valore del passo 4). (Domanda 5)
 *
 * OUTPUT ATTESO (i numeri dipendono dalla versione di Zephyr)
 *     Thread analyze:
 *      lavoratore          : STACK: unused ... usage ... / 4096 (.. %); CPU: .. %
 *      piccolo             : STACK: unused ... usage ... / 4096 (.. %); CPU: .. %
 *      ...
 *
 * CONCETTI NUOVI
 * - Memory region: totale di flash (codice + costanti) e RAM (variabili + stack + heap).
 * - ram_report / rom_report: chi occupa cosa, file per file, simbolo per simbolo.
 * - Thread analyzer: misura l'uso reale ("high water mark") di ogni stack.
 * - Margine di sicurezza: si misura il caso peggiore OSSERVATO, non quello possibile.
 *
 * COME VERIFICARE DA SOLO
 *     .\verifica.ps1 modulo7\06_memoria
 * controlla che l'analyzer stampi i due thread (anche dopo il ridimensionamento).
 *
 * ERRORI TIPICI
 * - Ridurre lo stack sotto l'uso misurato: errore fatale o comportamento strano.
 * - Misurare troppo presto: se la ricorsione non e' ancora avvenuta l'uso e' sottostimato.
 * - Cercare tabella_flash nel ram_report: e' const, sta in flash.
 *
 * BONUS (facoltativo)
 * - Togli "const" a tabella_flash: dove finisce? Quanta RAM in piu' usi? Perche' anche la
 *   flash resta occupata? (Suggerimento: i valori iniziali devono stare da qualche parte.)
 * - Porta PROFONDITA a 6 e rimisura: l'uso cresce di quanto ti aspetti
 *   (circa 128 byte di array + registri salvati per ogni livello)?
 * - "kernel thread stacks" dalla shell (Modulo 7 esercizio 1) da' gli stessi numeri?
 *
 * DOMANDE (rispondi qui)
 * 1. FLASH e RAM usate all'inizio:
 * 2. Byte di RAM di buffer_ram e di ciascuno stack nel ram_report:
 * 3. Byte di flash di tabella_flash nel rom_report:
 * 4. Uso misurato degli stack (lavoratore, piccolo) e dimensioni scelte:
 * 5. Cosa succede con uno stack di 256 byte?
 *
 * Il programma da misurare e' qui sotto: modifica solo le #define.
 */

#include <string.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

#define STACK_LAVORATORE 4096
#define STACK_PICCOLO    4096
#define DIM_BUFFER       8192
#define PROFONDITA       3

/* .bss: variabile globale non inizializzata -> RAM, azzerata all'avvio */
static uint8_t buffer_ram[DIM_BUFFER];

/* .rodata: costante -> resta in flash */
static const uint16_t tabella_flash[2048] = { 1, 2, 3, 5, 8, 13, 21, 34 };

/* Usa un po' di stack a ogni livello di ricorsione. */
static __noinline uint32_t usa_stack(int livello)
{
	volatile uint8_t locale[128];

	for (size_t i = 0; i < sizeof(locale); i++) {
		locale[i] = (uint8_t)(i + livello);
	}
	if (livello > 0) {
		return locale[livello] + usa_stack(livello - 1);
	}
	return locale[0];
}

static void lavoratore_fn(void *p1, void *p2, void *p3)
{
	uint32_t giro = 0;

	while (1) {
		uint32_t r = usa_stack(PROFONDITA);

		memset(buffer_ram, (int)(giro & 0xFF), sizeof(buffer_ram));
		r += tabella_flash[giro % ARRAY_SIZE(tabella_flash)];
		if (giro % 10 == 0) {
			printk("lavoratore: giro %u (r = %u)\n", giro, r);
		}
		giro++;
		k_msleep(500);
	}
}

static void piccolo_fn(void *p1, void *p2, void *p3)
{
	while (1) {
		k_msleep(1000);
	}
}

K_THREAD_DEFINE(lavoratore, STACK_LAVORATORE, lavoratore_fn, NULL, NULL, NULL, 7, 0, 0);
K_THREAD_DEFINE(piccolo, STACK_PICCOLO, piccolo_fn, NULL, NULL, NULL, 8, 0, 0);

int main(void)
{
	printk("Esercizio memoria: aspetta il rapporto del thread analyzer\n");
	return 0;
}
