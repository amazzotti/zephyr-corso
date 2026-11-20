/*
 * Esercizio 2 (Modulo 6) - Pulsante a interrupt
 *
 * Teoria: teoria/07_periferiche_driver.md, sezione 4.
 *
 * OBIETTIVO
 * Contare le pressioni di sw0 senza polling: e' il pulsante a "chiamare" il programma
 * con un interrupt. Il thread principale dorme finche' non c'e' una pressione, poi
 * stampa il totale e inverte led0.
 *
 * PASSI
 * 1. gpio_dt_spec per sw0 e led0 come nell'esercizio 1, con i controlli
 *    gpio_is_ready_dt e la configurazione (GPIO_INPUT, GPIO_OUTPUT_INACTIVE).
 * 2. Variabili globali:
 *        static struct gpio_callback cb_pulsante;   (la "registrazione" della callback)
 *        static atomic_t pressioni;                 (contatore condiviso con l'ISR)
 *        K_SEM_DEFINE(sem_pressione, 0, 10);        (segnale dall'ISR al thread)
 * 3. Scrivi la callback con questa firma esatta:
 *        static void pulsante_premuto(const struct device *dev,
 *                                     struct gpio_callback *cb, uint32_t pins)
 *    Dentro: atomic_inc(&pressioni) e k_sem_give(&sem_pressione). Nient'altro.
 * 4. In main(), dopo la configurazione del pin:
 *        gpio_pin_interrupt_configure_dt(&btn, GPIO_INT_EDGE_TO_ACTIVE);
 *        gpio_init_callback(&cb_pulsante, pulsante_premuto, BIT(btn.pin));
 *        gpio_add_callback(btn.port, &cb_pulsante);
 *    (controlla il valore restituito delle funzioni che restituiscono int).
 * 5. Stampa "Pronto: premi sw0".
 * 6. Ciclo infinito: k_sem_take(&sem_pressione, K_FOREVER); poi
 *        gpio_pin_toggle_dt(&led);
 *        printk("Pressioni: %d\n", (int)atomic_get(&pressioni));
 * 7. Prova in Renode premendo e rilasciando piu' volte (Press, Release, Press, ...).
 *
 * OUTPUT ATTESO (dopo tre pressioni)
 *     Pronto: premi sw0
 *     Pressioni: 1
 *     Pressioni: 2
 *     Pressioni: 3
 * Dopo un numero dispari di pressioni led0 e' acceso.
 *
 * CONCETTI NUOVI
 * - Interrupt e ISR: il codice gira "dentro" l'interrupt, interrompendo qualunque thread.
 * - GPIO_INT_EDGE_TO_ACTIVE: interrupt sul fronte in cui il pin diventa attivo (premuto).
 * - gpio_callback: la struttura con cui il driver sa quale tua funzione chiamare e per
 *   quali pin (BIT(btn.pin) e' la maschera con un solo bit acceso).
 * - Lavoro diviso: l'ISR fa il minimo (conta e segnala), il thread fa il resto (stampa).
 * - atomic_t: un intero che si puo' modificare dall'ISR e leggere dal thread senza race
 *   condition (Modulo 4).
 *
 * COME VERIFICARE DA SOLO
 *     .\verifica.ps1 modulo6\02_pulsante_interrupt
 * La verifica fa tre pressioni e controlla il conteggio e il LED.
 *
 * ERRORI TIPICI
 * - Firma della callback sbagliata (es. senza "const"): warning di tipi incompatibili
 *   in gpio_init_callback. Leggilo: non ignorarlo.
 * - Dichiarare cb_pulsante come variabile locale di una funzione che poi termina:
 *   il driver tiene un puntatore a una memoria che non esiste piu'. Deve essere
 *   static o globale.
 * - Chiamare k_msleep o printk "pesanti" nella callback: siamo in un interrupt.
 * - Dimenticare gpio_pin_interrupt_configure_dt: la callback non viene mai chiamata.
 * - GPIO_INT_EDGE_BOTH: conta due volte (pressione e rilascio).
 *
 * BONUS (facoltativo)
 * - Nella callback stampa k_is_in_isr() con printk (una volta sola, poi toglilo):
 *   che valore ha? E nel thread?
 * - Usa GPIO_INT_EDGE_BOTH e, nella callback, leggi lo stato del pin per distinguere
 *   pressione e rilascio: stampa anche "Rilasci: N".
 * - Una sola callback per sw0 e sw1: usa il parametro "pins" per capire quale e' stato
 *   premuto (pins & BIT(btn1.pin)).
 *
 * Scrivi il tuo codice qui sotto.
 */

#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/atomic.h>
#include <zephyr/sys/printk.h>

/* TODO: gpio_dt_spec, callback, contatore e semaforo (passi 1-3) */

int main(void)
{
	/* TODO: configurazione, registrazione della callback e ciclo (passi 1, 4-6) */

	return 0;
}
