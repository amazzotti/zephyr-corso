/*
 * Esercizio 5 (Modulo 7) - Debug con gdb
 *
 * Teoria: teoria/08_strumenti_debug_test.md, sezione 5.
 *
 * OBIETTIVO
 * Questa volta il codice e' gia' scritto, e contiene un BUG. Il programma legge 5
 * campioni, ne fa la media e la confronta con una soglia di 50, ma stampa:
 *     ERRORE: la soglia e' cambiata da sola!
 * Nessuna riga del programma scrive nella soglia... eppure cambia. Trova chi la
 * modifica usando il debugger, SENZA aggiungere printk.
 *
 * PASSI
 * 1. Compila e carica senza avviare:
 *        .\esegui.ps1 modulo7\05_debug_gdb -Pausa
 *    Nel monitor di Renode:
 *        machine StartGdbServer 3333
 * 2. Trova gdb. E' nello Zephyr SDK, nella cartella arm-zephyr-eabi\bin. Di solito
 *    l'SDK e' in %USERPROFILE%\zephyr-sdk-<versione>; se non c'e':
 *        Get-ChildItem -Path $env:USERPROFILE, C:\ -Filter arm-zephyr-eabi-gdb.exe -Recurse -ErrorAction SilentlyContinue -Depth 4
 * 3. In un secondo terminale (nella cartella del repo):
 *        & "<percorso>\arm-zephyr-eabi-gdb.exe" modulo7\05_debug_gdb\build\zephyr\zephyr.elf
 *    Poi, dentro gdb:
 *        (gdb) target remote :3333
 *        (gdb) break main
 *        (gdb) monitor start
 *        (gdb) continue
 *    gdb si ferma all'inizio di main.
 * 4. Guarda lo stato iniziale:
 *        (gdb) print stato
 *        (gdb) print stato.soglia
 *        (gdb) print &stato.soglia
 *        (gdb) print &stato.campioni[4]
 *    Quanti byte separano campioni[4] e soglia? (Domanda 1)
 * 5. Metti un WATCHPOINT: gdb si fermera' nel momento esatto in cui la soglia cambia.
 *        (gdb) watch stato.soglia
 *        (gdb) continue
 *    Su quale riga si ferma? Quanto vale i? (print i, info locals, bt)
 * 6. Correggi il bug (una sola modifica), ricompila, verifica.
 *
 * OUTPUT ATTESO (dopo la correzione)
 *     lettura 1: media = 30, soglia = 50
 *     lettura 2: media = 31, soglia = 50
 *     lettura 3: media = 32, soglia = 50
 *     lettura 4: media = 30, soglia = 50
 *     ...
 *
 * CONCETTI NUOVI
 * - Server gdb (in Renode o sulla sonda di debug della scheda vera) e client gdb.
 * - Breakpoint (fermati qui) e watchpoint (fermati quando questo valore cambia).
 * - Memoria contigua: in una struct i campi sono uno dopo l'altro. Scrivere oltre la
 *   fine di un array NON da' errore in C: sovrascrive cio' che c'e' dopo.
 * - Ottimizzazioni e debug: con -Og (CONFIG_DEBUG_OPTIMIZATIONS) le variabili si
 *   leggono; con -Os spesso risultano <optimized out>.
 *
 * COME VERIFICARE DA SOLO
 *     .\verifica.ps1 modulo7\05_debug_gdb
 * controlla le prime letture e che "ERRORE" non compaia mai.
 *
 * ERRORI TIPICI
 * - Avviare Renode senza -Pausa: il programma e' gia' passato oltre main quando gdb si
 *   collega (funziona lo stesso il watchpoint, ma ti perdi l'inizio).
 * - Usare il gdb "di sistema" (x86) invece di arm-zephyr-eabi-gdb: non capisce l'ARM.
 * - "Correggere" il sintomo (riscrivere stato.soglia = 50 nel ciclo) invece della causa.
 *
 * BONUS (facoltativo)
 * - x/6dw stato.campioni   mostra 6 parole da 4 byte a partire dall'array: vedi la
 *   soglia "in coda" all'array?
 * - Togli CONFIG_DEBUG_OPTIMIZATIONS, ricompila e ripeti: cosa cambia in gdb?
 * - Perche' leggi_campioni ha __noinline? (Suggerimento: prova a toglierlo e guarda i
 *   warning del compilatore: con il ciclo "visibile" il compilatore puo' accorgersi
 *   dell'errore... o peggio, ottimizzare in modi strani.)
 *
 * DOMANDE (rispondi qui)
 * 1. Distanza in byte tra &stato.campioni[4] e &stato.soglia:
 * 2. Riga in cui il watchpoint si ferma e valore di i:
 * 3. Il bug in una frase:
 * 4. Perche' il compilatore non ti ha avvisato?
 *
 * Il codice qui sotto e' gia' scritto: correggilo dopo aver trovato il bug con gdb.
 */

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

#define N_CAMPIONI      5
#define SOGLIA_INIZIALE 50

static struct {
	int32_t campioni[N_CAMPIONI];
	int32_t soglia;
	uint32_t letture;
} stato = { .soglia = SOGLIA_INIZIALE };

/* Simula la lettura di n campioni da un sensore. */
static __noinline void leggi_campioni(int32_t *dst, size_t n)
{
	for (size_t i = 0; i <= n; i++) {
		dst[i] = (int32_t)(10 * (i + 1) + (stato.letture % 3));
	}
	stato.letture++;
}

static int32_t media(const int32_t *v, size_t n)
{
	int32_t somma = 0;

	for (size_t i = 0; i < n; i++) {
		somma += v[i];
	}
	return somma / (int32_t)n;
}

int main(void)
{
	while (1) {
		leggi_campioni(stato.campioni, N_CAMPIONI);
		int32_t m = media(stato.campioni, N_CAMPIONI);

		printk("lettura %u: media = %d, soglia = %d\n",
		       stato.letture, m, stato.soglia);

		if (stato.soglia != SOGLIA_INIZIALE) {
			printk("ERRORE: la soglia e' cambiata da sola!\n");
		}
		if (m > stato.soglia) {
			printk("ALLARME: media sopra la soglia\n");
		}
		k_msleep(1000);
	}
	return 0;
}
