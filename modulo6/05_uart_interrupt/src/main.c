/*
 * Esercizio 5 (Modulo 6) - UART a interrupt
 *
 * Teoria: teoria/07_periferiche_driver.md, sezioni 4 e 6;
 *         teoria/05_sincronizzazione.md (k_msgq).
 *
 * OBIETTIVO
 * Ricevere righe di testo dalla UART con gli interrupt (nessun polling) e usarle come
 * comandi per il LED:
 *     on     -> accende led0 e stampa "LED acceso"
 *     off    -> spegne led0 e stampa "LED spento"
 *     stato  -> stampa "LED: acceso" oppure "LED: spento"
 *     altro  -> stampa "Comando sconosciuto: <riga>"
 *
 * PASSI
 * 1. Device della console (DT_CHOSEN(zephyr_console)) e gpio_dt_spec di led0, con i
 *    soliti controlli; LED configurato GPIO_OUTPUT_INACTIVE.
 * 2. Una coda di righe complete, dall'interrupt al thread:
 *        #define MAX_RIGA 32
 *        K_MSGQ_DEFINE(coda_righe, MAX_RIGA, 4, 1);
 *    Ogni messaggio e' un array di MAX_RIGA char (la riga con il suo '\0').
 * 3. La callback della UART:
 *        static void uart_cb(const struct device *dev, void *user_data)
 *    - uart_irq_update(dev);  (va chiamata una volta all'inizio di ogni callback: aggiorna
 *      lo stato degli interrupt letto dalle funzioni seguenti; non restituisce nulla)
 *    - while (uart_irq_rx_ready(dev)): leggi un byte con uart_fifo_read(dev, &c, 1);
 *        * se e' '\r' o '\n' e la riga non e' vuota: chiudi con '\0' e mettila in coda
 *          con k_msgq_put(&coda_righe, riga, K_NO_WAIT), poi azzera la lunghezza;
 *        * altrimenti aggiungi c alla riga, se c'e' posto.
 *    Il buffer della riga in costruzione e la sua lunghezza sono static (sopravvivono
 *    tra una chiamata e l'altra). NIENTE eco e niente printk nella callback.
 * 4. In main():
 *        uart_irq_callback_user_data_set(uart, uart_cb, NULL);
 *        uart_irq_rx_enable(uart);
 *    poi stampa "Comandi: on, off, stato".
 * 5. Ciclo infinito: k_msgq_get(&coda_righe, riga, K_FOREVER), poi confronta con
 *    strcmp(riga, "on") == 0 ecc. ed esegui il comando.
 *
 * OUTPUT ATTESO
 *     Comandi: on, off, stato
 *     LED acceso            (dopo "on")
 *     LED: acceso           (dopo "stato")
 *     LED spento            (dopo "off")
 *     Comando sconosciuto: ciao
 *
 * CONCETTI NUOVI
 * - Interrupt della UART: la callback viene chiamata quando c'e' un byte ricevuto
 *   (sull'STM32: bit RXNE del registro di stato della USART2, interrupt USART2 nel
 *   NVIC); uart_irq_update + uart_irq_rx_ready + uart_fifo_read e' lo schema standard.
 * - Il buffer di riga lo riempie l'ISR; il thread riceve solo righe COMPLETE tramite
 *   una k_msgq (messaggi di dimensione fissa, copiati: nessun problema di condivisione).
 * - strcmp (in <string.h>): confronto tra stringhe C. 0 = uguali.
 *
 * COME VERIFICARE DA SOLO
 *     .\verifica.ps1 modulo6\05_uart_interrupt
 *
 * ERRORI TIPICI
 * - Confrontare le stringhe con ==: confronta gli INDIRIZZI, non il testo.
 * - Dimenticare uart_irq_rx_enable: la callback non viene mai chiamata.
 * - Mancanza di CONFIG_UART_INTERRUPT_DRIVEN=y: errore di link su uart_irq_... (oppure
 *   le funzioni restituiscono -ENOSYS).
 * - k_msgq_put con K_FOREVER nell'ISR: in interrupt non si puo' aspettare.
 * - Righe che finiscono con '\r' ancora dentro: "on\r" non e' uguale a "on".
 *
 * BONUS (facoltativo)
 * - Comando "blink N": fa lampeggiare il LED N volte (sscanf o strtol per il numero).
 * - Eco dei caratteri digitati: NON con printk nella callback, ma con un
 *   k_msgq di caratteri e un thread che li rimanda indietro. Perche' cosi'?
 * - Fai riempire la coda (4 righe) mentre il thread e' occupato (aggiungi k_msleep(2000)
 *   dopo ogni comando) e conta le righe perse quando k_msgq_put fallisce.
 *
 * Scrivi il tuo codice qui sotto.
 */

#include <string.h>
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/sys/printk.h>

/* TODO: device, LED, coda delle righe e callback della UART (passi 1-3) */

int main(void)
{
	/* TODO: attivazione degli interrupt e ciclo dei comandi (passi 4-5) */

	return 0;
}
