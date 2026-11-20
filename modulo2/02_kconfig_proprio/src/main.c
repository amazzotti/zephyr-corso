/*
 * Esercizio 2 (Modulo 2) - Opzioni Kconfig proprie
 *
 * Teoria: teoria/03_build_kconfig_devicetree.md, sezioni 2 e 3.
 *
 * OBIETTIVO
 * Definire tre opzioni di configurazione tue (una stringa, un intero, un booleano)
 * nel file Kconfig, impostarle in prj.conf e usarle nel codice per stampare un
 * saluto ripetuto.
 *
 * PASSI
 * 1. Guarda il file Kconfig: c'e' gia' CORSO_NOME (stringa). In prj.conf e' impostata a
 *    "Zephyr". In main() stampa:
 *        printk("%s, %s!\n", "Ciao", CONFIG_CORSO_NOME);
 *    Esegui: deve comparire "Ciao, Zephyr!".
 * 2. Nel file Kconfig aggiungi CORSO_RIPETIZIONI (int, default 3, range 1 10).
 *    Usala come limite di un ciclo for che stampa il saluto con il numero del giro:
 *        Ciao, Zephyr! (1/3)
 *        Ciao, Zephyr! (2/3)
 *        Ciao, Zephyr! (3/3)
 *    Il ciclo for in C:  for (int i = 1; i <= N; i++) { ... }
 *    (inizializzazione; condizione per continuare; cosa fare a fine giro)
 * 3. In prj.conf imposta CONFIG_CORSO_RIPETIZIONI=4 e riesegui: 4 righe.
 *    Prova anche 20: cosa dice la compilazione? (Il range non lo permette: leggi
 *    l'avviso.) Rimetti 4.
 * 4. Aggiungi CORSO_FORMALE (bool, default n). Se e' attiva, il saluto diventa
 *    "Buongiorno" invece di "Ciao". Usa IS_ENABLED(CONFIG_CORSO_FORMALE).
 *    Imposta CONFIG_CORSO_FORMALE=y in prj.conf.
 * 5. Apri build/zephyr/.config e cerca CORSO_: ci sono le tue tre opzioni?
 * 6. Lancia la verifica (con le impostazioni finali: 4 ripetizioni, formale).
 *
 * OUTPUT ATTESO (finale)
 *     Buongiorno, Zephyr! (1/4)
 *     Buongiorno, Zephyr! (2/4)
 *     Buongiorno, Zephyr! (3/4)
 *     Buongiorno, Zephyr! (4/4)
 *
 * CONCETTI NUOVI
 * - File Kconfig dell'applicazione: definisce opzioni con tipo, default, range, aiuto.
 * - source "Kconfig.zephyr": senza questa riga in fondo spariscono tutte le opzioni
 *   di Zephyr e la compilazione fallisce.
 * - Ciclo for del C.
 * - Una stringa Kconfig e' una macro che vale "testo" (con le virgolette): si stampa
 *   con %s.
 *
 * COME VERIFICARE DA SOLO
 *     .\verifica.ps1 modulo2\02_kconfig_proprio -Pulito
 * (-Pulito perche' hai cambiato Kconfig: e' piu' sicuro ricompilare da zero.)
 *
 * ERRORI TIPICI
 * - Scrivere "config CONFIG_CORSO_..." nel file Kconfig: il prefisso NON va.
 * - Indentare con spazi invece che con tab nel file Kconfig.
 * - Impostare in prj.conf un'opzione non ancora definita in Kconfig: errore
 *   "attempt to assign the value ... to the undefined symbol".
 * - Stringa in prj.conf senza virgolette: CONFIG_CORSO_NOME=Zephyr (sbagliato).
 * - Ciclo da 0 a < N che stampa (0/4)...(3/4): conta da 1.
 *
 * BONUS (facoltativo)
 * - Aggiungi "depends on CORSO_FORMALE" a una nuova opzione stringa CORSO_TITOLO
 *   (es. "Ing.") e stampala solo se esiste: cosa succede in .config se CORSO_FORMALE=n?
 * - Lancia  west build -t menuconfig -d modulo2\02_kconfig_proprio\build  e trova il
 *   menu "Esercizio Kconfig proprio" con le tue opzioni.
 *
 * Scrivi il tuo codice qui sotto.
 */

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys/util.h>

int main(void)
{
	/* TODO: passi 1, 2 e 4 */

	return 0;
}
