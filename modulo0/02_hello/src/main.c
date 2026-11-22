/*
 * Esercizio 2 (Modulo 0) - Ciao, Zephyr!
 *
 * Teoria: teoria/01_setup_toolchain.md, sezioni 6, 7 e 8.
 *
 * OBIETTIVO
 * Il primo firmware: stampare sulla UART della scheda simulata un saluto e il nome
 * della scheda.
 *
 * PASSI
 * 1. Prima di scrivere codice, compila ed esegui lo scheletro cosi' com'e':
 *        .\esegui.ps1 modulo0\02_hello
 *    Nella finestra della UART deve comparire solo il banner di avvio
 *        *** Booting Zephyr OS build ... ***
 *    Chiudi Renode con  quit  nel monitor.
 * 2. Dentro main(), prima di "return 0;", aggiungi:
 *        printk("Ciao, Zephyr!\n");
 *    "\n" e' il ritorno a capo: senza, la riga potrebbe non comparire subito.
 * 3. Aggiungi una seconda riga che stampi il nome della scheda. Zephyr lo mette nella
 *    macro CONFIG_BOARD (una stringa decisa in compilazione):
 *        printk("Scheda: %s\n", CONFIG_BOARD);
 *    %s nel formato viene sostituito dalla stringa passata dopo la virgola.
 * 4. Esegui di nuovo e poi lancia la verifica automatica:
 *        .\verifica.ps1 modulo0\02_hello
 *
 * OUTPUT ATTESO
 *     *** Booting Zephyr OS build v4.x.x ***
 *     Ciao, Zephyr!
 *     Scheda: stm32f4_disco
 *
 * CONCETTI NUOVI
 * - #include: inserisce le dichiarazioni di una libreria (qui il kernel e printk).
 *   E' l'equivalente dell'import di Python.
 * - int main(void): il punto di partenza del TUO codice. Zephyr la chiama dopo aver
 *   inizializzato kernel e driver. "void" = nessun parametro.
 * - printk(formato, valori...): stampa sulla console (la UART).
 * - Ogni istruzione C finisce con ";". I blocchi sono tra { } (non l'indentazione,
 *   come in Python: l'indentazione serve solo a chi legge).
 * - CONFIG_BOARD: una delle tante costanti CONFIG_... generate da Kconfig (Modulo 2).
 *
 * COME VERIFICARE DA SOLO
 *     .\verifica.ps1 modulo0\02_hello
 * deve finire con "TUTTO OK". Le righe controllate sono in verifica.txt.
 *
 * ERRORI TIPICI
 * - Dimenticare il ";" in fondo alla riga: "error: expected ';' before ...".
 *   L'errore e' segnalato spesso sulla riga DOPO quella sbagliata.
 * - Virgolette sbagliate: in C le stringhe vogliono le doppie "...". Le singole '.'
 *   sono per un solo carattere.
 * - Scrivere printk dopo "return 0;": non viene mai eseguito.
 * - Dimenticare "\n": il testo resta "appeso" in attesa della fine della riga.
 *
 * BONUS (facoltativo)
 * - Stampa anche CONFIG_SOC (il nome del chip) e CONFIG_ARCH (l'architettura).
 * - Apri build/zephyr/zephyr.elf? No: e' binario. Guarda invece build/zephyr/zephyr.map
 *   con VS Code e cerca "main": a che indirizzo di memoria e' finita la tua funzione?
 * - Dopo la compilazione, west stampa la tabella "Memory region": quanta FLASH e quanta
 *   RAM usa questo firmware?
 *
 * Scrivi il tuo codice qui sotto.
 */

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

int main(void)
{
	/* Scrivi qui il tuo codice. */

	return 0;
}
