/*
 * Esercizio 4 (Modulo 6) - UART in polling
 *
 * Teoria: teoria/07_periferiche_driver.md, sezione 6.
 *
 * OBIETTIVO
 * Leggere i caratteri che scrivi nella finestra della UART di Renode, rimandarli
 * indietro (eco, cosi' vedi cosa scrivi) e, quando premi Invio, stampare la riga
 * completa con la sua lunghezza.
 *
 * PASSI
 * 1. Prendi il device della console:
 *        static const struct device *uart = DEVICE_DT_GET(DT_CHOSEN(zephyr_console));
 *    e controlla device_is_ready(uart). E' la USART2 dell'STM32 (pin PA2 = TX,
 *    PA3 = RX), la stessa usata da printk: la parte che trasmette (TX) la usano tutti
 *    e due, la parte che riceve (RX) solo tu.
 * 2. Stampa "Scrivi una riga e premi Invio".
 * 3. Prepara un buffer di riga:
 *        #define MAX_RIGA 64
 *        char riga[MAX_RIGA];
 *        size_t len = 0;
 * 4. Ciclo infinito:
 *      - unsigned char c; se uart_poll_in(uart, &c) == 0 c'e' un carattere, altrimenti
 *        (-1) non c'e' niente: k_msleep(5) e ricomincia;
 *      - se c e' '\r' oppure '\n': se len > 0, termina la stringa (riga[len] = '\0'),
 *        stampa  "\r\nRiga (%u caratteri): %s\n"  e azzera len. Se len == 0 (riga
 *        vuota, per esempio il '\n' dopo un '\r') non fare niente;
 *      - altrimenti, se c'e' posto (len < MAX_RIGA - 1): metti c nel buffer e fai l'eco
 *        con uart_poll_out(uart, c). Se non c'e' posto, ignora il carattere.
 * 5. Esegui con .\esegui.ps1, clicca nella finestra della UART e scrivi.
 *
 * OUTPUT ATTESO (scrivendo "ciao zephyr" e Invio)
 *     Scrivi una riga e premi Invio
 *     ciao zephyr
 *     Riga (11 caratteri): ciao zephyr
 *
 * CONCETTI NUOVI
 * - DT_CHOSEN(zephyr_console): il nodo che il devicetree "sceglie" come console.
 * - uart_poll_in / uart_poll_out: API a polling, un carattere alla volta.
 * - '\r' (carriage return, codice 13) e '\n' (line feed, codice 10): cosa manda Invio.
 * - Stringhe C costruite a mano: il terminatore '\0' e il controllo dello spazio
 *   (Modulo 1: buffer overflow).
 *
 * COME VERIFICARE DA SOLO
 *     .\verifica.ps1 modulo6\04_uart_polling
 * La verifica scrive due righe e controlla le risposte.
 *
 * ERRORI TIPICI
 * - Dimenticare riga[len] = '\0': printk("%s") stampa anche la "spazzatura" che segue.
 * - Nessun controllo su len: con una riga lunga scrivi oltre la fine del buffer e
 *   rovini altre variabili (o lo stack).
 * - Ciclo senza k_msleep: la CPU e' sempre occupata a chiedere "c'e' un carattere?".
 * - %u con size_t: usa (unsigned int)len per evitare il warning di formato
 *   (oppure %zu).
 *
 * BONUS (facoltativo)
 * - Gestisci il tasto Backspace (codice 8 o 127): togli l'ultimo carattere dal buffer
 *   e manda alla UART la sequenza "\b \b" per cancellarlo anche a video.
 * - Porta k_msleep(5) a k_msleep(500) e incolla una frase lunga nella finestra della
 *   UART: arriva tutta? Perche' no? (Pensa a quanti caratteri tiene la UART da sola.)
 *
 * Scrivi il tuo codice qui sotto.
 */

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/sys/printk.h>

/* TODO: device della console (passo 1) */

int main(void)
{
	/* TODO: buffer di riga e ciclo di lettura (passi 2-4) */

	return 0;
}
