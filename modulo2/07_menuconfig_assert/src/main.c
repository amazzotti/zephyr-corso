/*
 * Esercizio 7 (Modulo 2) - menuconfig e asserzioni
 *
 * Teoria: teoria/03_build_kconfig_devicetree.md, sezione 9.
 *
 * OBIETTIVO
 * Esplorare le opzioni Kconfig con menuconfig, poi usare __ASSERT per scoprire subito
 * un errore di programmazione (una divisione per zero) che altrimenti passerebbe
 * in silenzio.
 *
 * PASSI
 * 1. Scrivi la funzione  static int dividi(int a, int b)  che restituisce a / b.
 *    In main() stampa:
 *        volatile int zero = 0;
 *        printk("Risultato: %d\n", dividi(10, 2));
 *        printk("Risultato: %d\n", dividi(10, zero));
 *    (volatile impedisce al compilatore di "vedere" in anticipo che il divisore e' 0:
 *    altrimenti potrebbe trasformare il codice in modi imprevedibili. Modulo 1.)
 *    Esegui. La seconda riga stampa "Risultato: 0": il Cortex-M4, di default,
 *    restituisce 0 dividendo per zero, SENZA errori. Un bug silenzioso.
 * 2. Esplora menuconfig (dopo aver compilato almeno una volta):
 *        west build -t menuconfig -d modulo2\07_menuconfig_assert\build
 *    Rispondi alle domande del file menuconfig.md (frecce per muoversi, Invio per
 *    entrare, "/" per cercare, "?" per l'aiuto, Esc Esc per uscire, salva con S).
 *    Se menuconfig non parte su Windows, prova  -t guiconfig  (finestra grafica).
 * 3. In dividi(), prima della divisione, aggiungi:
 *        __ASSERT(b != 0, "divisore nullo (a = %d)", a);
 *    Senza CONFIG_ASSERT l'asserzione non fa nulla: provalo.
 * 4. Attiva le asserzioni con l'opzione trovata al passo 2, scrivendola in prj.conf.
 *    Ricompila con -Pulito ed esegui: ora il firmware si ferma con un messaggio che
 *    dice file, riga e condizione fallita, seguito da un errore fatale.
 * 5. Lancia la verifica.
 *
 * OUTPUT ATTESO (finale)
 *     Risultato: 5
 *     ASSERTION FAIL [b != 0] @ .../src/main.c:NN
 *         divisore nullo (a = 10)
 *     ... ZEPHYR FATAL ERROR ...
 *
 * CONCETTI NUOVI
 * - menuconfig: interfaccia per esplorare le opzioni, le loro dipendenze e l'aiuto.
 *   Le modifiche valgono solo per quella cartella build: quelle da tenere vanno in
 *   prj.conf.
 * - __ASSERT(condizione, messaggio, ...): "qui deve essere vero, altrimenti c'e' un bug".
 *   Con CONFIG_ASSERT=n non genera codice.
 * - Errore fatale: il kernel ferma il sistema e stampa lo stato della CPU (Modulo 7).
 * - Funzioni "static": visibili solo in questo file (Modulo 1).
 *
 * COME VERIFICARE DA SOLO
 *     .\verifica.ps1 modulo2\07_menuconfig_assert -Pulito
 *
 * ERRORI TIPICI
 * - Mettere l'__ASSERT DOPO la divisione: troppo tardi.
 * - Cambiare l'opzione solo in menuconfig e poi compilare con -Pulito: la modifica si
 *   perde. Va scritta in prj.conf.
 * - Usare __ASSERT per errori che possono capitare davvero (un sensore scollegato):
 *   quelli si gestiscono con i codici di ritorno, il sistema non deve fermarsi.
 *
 * BONUS (facoltativo)
 * - Usa __ASSERT_NO_MSG(b != 0): che differenza c'e' nel messaggio?
 * - Cerca in menuconfig l'opzione per far scattare un'eccezione hardware nella
 *   divisione per zero (cerca "DIV" o leggi l'help delle opzioni ARM). Il Cortex-M4 ha
 *   un bit di configurazione (DIV_0_TRP) che lo fa.
 *
 * Scrivi il tuo codice qui sotto.
 */

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys/__assert.h>

/* TODO: passo 1 - la funzione dividi */

int main(void)
{
	/* TODO: passo 1 */

	return 0;
}
