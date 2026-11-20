/*
 * Esercizio 1 (Modulo 7) - La shell
 *
 * Teoria: teoria/08_strumenti_debug_test.md, sezione 1.
 *
 * OBIETTIVO
 * Attivare la shell di Zephyr e usarla per guardare "dentro" il firmware mentre gira:
 * versione, tempo dall'avvio, thread, dispositivi, logging. Nel frattempo un thread
 * scrive un messaggio di "battito" ogni 5 secondi.
 *
 * PASSI
 * 1. In prj.conf aggiungi:
 *        CONFIG_SHELL=y          # la shell sulla UART della console
 *        CONFIG_THREAD_NAME=y    # i thread hanno un nome (lo vedi in "kernel thread list")
 *    Ricompila con -Pulito (hai cambiato la configurazione):
 *        .\esegui.ps1 modulo7\01_shell -Pulito
 *    Nella finestra della UART compare il prompt  uart:~$
 *    Clicca nella finestra della UART per scriverci dentro.
 * 2. Prova i comandi (vedi la tabella nella teoria) e rispondi alle domande in fondo
 *    a questo file:
 *        help
 *        kernel version
 *        kernel uptime
 *        kernel thread list      (vecchie versioni: kernel threads)
 *        kernel thread stacks    (vecchie versioni: kernel stacks)
 *        device list
 *        log status
 *    Prova anche il tasto Tab (completamento) e le frecce su/giu' (storico).
 * 3. Scrivi un thread "battito" con K_THREAD_DEFINE (Modulo 3) che ogni 5 secondi scrive
 *        LOG_INF("battito %u", n);
 *    con n che parte da 1. Stack 1024, priorita' 7.
 * 4. Con il firmware in esecuzione:
 *        log disable app       -> i battiti spariscono (il thread continua a girare!)
 *        log enable inf app    -> ricompaiono
 *    Il nome "app" e' quello di LOG_MODULE_REGISTER qui sotto.
 * 5. Verifica: .\verifica.ps1 modulo7\01_shell
 *
 * OUTPUT ATTESO (i numeri cambiano)
 *     *** Booting Zephyr OS build v4.x.x ***
 *     uart:~$ kernel version
 *     Zephyr version 4.x.x
 *     uart:~$ [00:00:05.000,000] <inf> app: battito 1
 *     uart:~$ [00:00:10.000,000] <inf> app: battito 2
 *
 * CONCETTI NUOVI
 * - Shell: una riga di comando che gira SUL microcontrollore. I comandi sono funzioni
 *   del firmware, compilate dentro zephyr.elf.
 * - La shell ha un suo thread: puoi usarla mentre gli altri thread lavorano.
 * - Logging a runtime: il livello di ogni modulo si puo' cambiare senza ricompilare.
 * - Con la shell attiva si usa LOG_* invece di printk (la teoria spiega perche').
 *
 * COME VERIFICARE DA SOLO
 *     .\verifica.ps1 modulo7\01_shell
 * esegue alcuni comandi della shell e aspetta il primo battito.
 *
 * ERRORI TIPICI
 * - Dimenticare -Pulito dopo aver cambiato prj.conf: a volte la vecchia configurazione
 *   resta. Nel dubbio, ricompila da zero.
 * - Scrivere nella finestra del MONITOR di Renode invece che in quella della UART: il
 *   monitor risponde "No such command".
 * - Thread senza ciclo infinito: stampa una volta e termina.
 * - Usare printk nel thread: funziona, ma il testo si mescola al prompt.
 *
 * BONUS (facoltativo)
 * - "kernel thread list": quanti thread ci sono? Chi li ha creati (tu, la shell, il
 *   logging, il kernel)? Qual e' il thread "idle" e che priorita' ha?
 * - Aggiungi CONFIG_SHELL_PROMPT_UART="nodo:~$ " e ricompila.
 * - Confronta la tabella "Memory region" con e senza CONFIG_SHELL: quanta FLASH e quanta
 *   RAM costa la shell?
 *
 * DOMANDE (rispondi qui, dentro il commento)
 * 1. Quanti thread sono in esecuzione e come si chiamano?
 * 2. Quanto stack usa al massimo il thread della shell? Su quanto disponibile?
 * 3. Quali dispositivi elenca "device list"? Riconosci gpio0, uart0, i2c0?
 * 4. Dopo "log disable app" il thread battito continua a girare? Come lo verifichi?
 *
 * Scrivi il tuo codice qui sotto.
 */

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(app, LOG_LEVEL_INF);

/* TODO: thread "battito" con K_THREAD_DEFINE. */

int main(void)
{
	LOG_INF("Shell pronta: scrivi help");
	return 0;
}
