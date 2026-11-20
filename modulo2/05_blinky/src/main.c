/*
 * Esercizio 5 (Modulo 2) - Il primo LED (blinky)
 *
 * Teoria: teoria/03_build_kconfig_devicetree.md, sezione 7.
 *
 * OBIETTIVO
 * Il "ciao mondo" dell'embedded: far lampeggiare il LED 0 della scheda, un secondo
 * acceso e un secondo spento, usando l'API GPIO di Zephyr e il devicetree.
 *
 * PASSI
 * 1. La variabile "led" qui sotto e' gia' pronta: GPIO_DT_SPEC_GET legge dal
 *    devicetree porta, pin e flag del nodo con alias led0.
 * 2. In main(), prima di tutto controlla che il driver della porta sia pronto:
 *        if (!gpio_is_ready_dt(&led)) {
 *            printk("GPIO non pronta\n");
 *            return 0;
 *        }
 * 3. Configura il pin come uscita, inizialmente spento:
 *        int ret = gpio_pin_configure_dt(&led, GPIO_OUTPUT_INACTIVE);
 *    Se ret < 0 stampa "Errore configurazione: %d" e termina.
 * 4. Ciclo infinito  while (1) { ... }  che:
 *      - accende il LED con gpio_pin_set_dt(&led, 1), stampa "LED acceso",
 *        aspetta 1000 ms con k_msleep(1000);
 *      - spegne con gpio_pin_set_dt(&led, 0), stampa "LED spento", aspetta 1000 ms.
 * 5. In Renode, mentre gira, scrivi nel monitor:  sysbus.gpio0.led0 State
 *    piu' volte: vedi True e False alternarsi.
 * 6. Lancia la verifica.
 *
 * OUTPUT ATTESO
 *     LED acceso
 *     LED spento
 *     LED acceso
 *     ...
 *
 * CONCETTI NUOVI
 * - Driver e dispositivo: la porta GPIO e' un "device" gestito da un driver che
 *   Zephyr inizializza prima di main(). gpio_is_ready_dt controlla che sia andata bene.
 * - gpio_dt_spec: porta + pin + flag, tutto dal devicetree. Il codice non contiene
 *   "pin 13": cambiando scheda (o overlay) lo stesso codice funziona.
 * - Valore logico contro livello elettrico: il LED e' attivo basso, ma con le
 *   funzioni _dt scrivi 1 = acceso. Sotto, il driver scrive 0 nel bit 13 del
 *   registro OUT della porta gpio0.
 * - Codici di ritorno: 0 = ok, negativo = errore (convenzione di tutto Zephyr).
 * - while (1): il ciclo infinito di ogni firmware.
 *
 * COME VERIFICARE DA SOLO
 *     .\verifica.ps1 modulo2\05_blinky
 * Controlla le righe stampate e, subito dopo ognuna, lo stato del LED in Renode.
 *
 * ERRORI TIPICI
 * - Stampare "LED acceso" PRIMA di accenderlo: la verifica legge lo stato appena vede
 *   la riga, quindi accendi prima e stampa dopo.
 * - Dimenticare gpio_pin_configure_dt: il pin resta un ingresso e il LED non cambia.
 * - Passare "led" invece di "&led": le funzioni vogliono l'indirizzo della struttura.
 * - Ciclo senza k_msleep: il LED cambia milioni di volte al secondo e la CPU non
 *   dorme mai.
 *
 * BONUS (facoltativo)
 * - Usa gpio_pin_toggle_dt al posto delle due gpio_pin_set_dt.
 * - Fai lampeggiare insieme led0 e led1, in opposizione (uno acceso, l'altro spento).
 * - Fai un "SOS" in codice Morse con tempi diversi.
 *
 * Scrivi il tuo codice qui sotto.
 */

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/drivers/gpio.h>

static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(DT_ALIAS(led0), gpios);

int main(void)
{
	/* TODO: passi 2, 3 e 4 */

	return 0;
}
