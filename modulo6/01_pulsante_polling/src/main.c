/*
 * Esercizio 1 (Modulo 6) - Pulsante in polling
 *
 * Teoria: teoria/07_periferiche_driver.md, sezioni 1, 2 e 3.
 *
 * OBIETTIVO
 * Leggere il pulsante sw0 a intervalli regolari (polling) e far seguire al LED led0
 * lo stato del pulsante: premuto = acceso, rilasciato = spento. Stampare un messaggio
 * solo quando lo stato CAMBIA.
 *
 * PASSI
 * 1. Prendi dal devicetree i due collegamenti:
 *        static const struct gpio_dt_spec btn = GPIO_DT_SPEC_GET(DT_ALIAS(sw0), gpios);
 *        static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(DT_ALIAS(led0), gpios);
 *    Apri build/zephyr/zephyr.dts e cerca "button0": guarda i flag
 *    (GPIO_PULL_UP | GPIO_ACTIVE_LOW) e il numero di pin (11).
 * 2. In main() controlla con gpio_is_ready_dt() che entrambi siano pronti; se no stampa
 *    "Errore: GPIO non pronto" e termina con return 0.
 * 3. Configura btn come GPIO_INPUT e led come GPIO_OUTPUT_INACTIVE. Controlla il
 *    valore restituito (0 = ok, negativo = errore).
 * 4. Stampa "Pronto: premi sw0".
 * 5. Ciclo infinito:
 *      - leggi lo stato con gpio_pin_get_dt(&btn)  (1 = premuto, 0 = rilasciato);
 *      - imposta il LED con gpio_pin_set_dt(&led, stato);
 *      - SOLO se lo stato e' diverso dal giro precedente stampa "Premuto" oppure
 *        "Rilasciato" (serve una variabile con lo stato precedente);
 *      - k_msleep(10): il periodo di polling.
 * 6. Esegui con .\esegui.ps1 modulo6\01_pulsante_polling e nel monitor di Renode:
 *        sysbus.gpio0.sw0 Press
 *        sysbus.gpio0.led0 State
 *        sysbus.gpio0.sw0 Release
 *        sysbus.gpio0.led0 State
 *
 * OUTPUT ATTESO
 *     Pronto: premi sw0
 *     Premuto
 *     Rilasciato
 * e  sysbus.gpio0.led0 State  risponde True mentre il pulsante e' premuto, False dopo.
 *
 * CONCETTI NUOVI
 * - struct gpio_dt_spec: porta + pin + flag presi dal devicetree in un solo oggetto.
 * - GPIO_INPUT, GPIO_OUTPUT_INACTIVE: direzione del pin (e valore iniziale dell'uscita).
 * - Logica "attiva": gpio_pin_get_dt restituisce 1 = premuto anche se il pin
 *   elettricamente e' a 0 (pulsante attivo basso). Il flag GPIO_ACTIVE_LOW del DT
 *   fa l'inversione per te.
 * - Polling e rilevamento del cambiamento (fronte) fatto in software.
 *
 * COME VERIFICARE DA SOLO
 *     .\verifica.ps1 modulo6\01_pulsante_polling
 * La verifica preme e rilascia sw0 e controlla messaggi e stato del LED.
 *
 * ERRORI TIPICI
 * - Stampare a ogni giro del ciclo: 100 righe al secondo. Si stampa solo sul cambiamento.
 * - Invertire a mano il valore letto (!stato): con i flag del DT e' gia' giusto, e il LED
 *   si accende al contrario.
 * - Dimenticare k_msleep: il ciclo occupa la CPU al 100% (gli altri thread, compreso
 *   quello che stampa i log, non girano mai).
 * - Non controllare il valore restituito: gpio_pin_get_dt puo' restituire un errore
 *   negativo, che come "stato" sarebbe considerato vero.
 *
 * BONUS (facoltativo)
 * - Usa tutti e quattro i pulsanti e i quattro LED (sw1->led1, ...) con due array di
 *   gpio_dt_spec e un ciclo for.
 * - Porta il periodo di polling a 500 ms e prova a premere e rilasciare molto in fretta
 *   (due comandi di fila nel monitor): il programma se ne accorge? Perche'?
 *
 * Scrivi il tuo codice qui sotto.
 */

#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/printk.h>

/* TODO: le due gpio_dt_spec (passo 1) */

int main(void)
{
	/* TODO: controlli, configurazione e ciclo di polling (passi 2-5) */

	return 0;
}
