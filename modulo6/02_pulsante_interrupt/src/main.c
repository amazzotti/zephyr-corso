/*
 * Esercizio 2 (Modulo 6) - Pulsante a interrupt
 *
 * Teoria: teoria/07_periferiche_driver.md, sezione 4;
 *         teoria/06_tempo_lavoro_differito.md (contesto di interrupt).
 *
 * OBIETTIVO
 * Contare le pressioni del pulsante sw0 SENZA leggerlo continuamente: e' il pulsante
 * stesso, tramite un interrupt, a chiamare il tuo codice quando viene premuto.
 * A ogni pressione il LED verde led0 cambia stato e il programma stampa il totale.
 *
 * PASSI
 * 1. gpio_dt_spec di sw0 e led0 (come nell'esercizio 1), con i controlli
 *    gpio_is_ready_dt. Configura sw0 come GPIO_INPUT e led0 come GPIO_OUTPUT_INACTIVE.
 * 2. Il semaforo che collega l'interrupt al thread:
 *        K_SEM_DEFINE(sem_pressione, 0, 100);
 *    e un contatore condiviso:
 *        static atomic_t pressioni = ATOMIC_INIT(0);
 * 3. La callback (gira in contesto di INTERRUPT: breve, niente attese):
 *        static void pulsante_premuto(const struct device *dev,
 *                                     struct gpio_callback *cb, uint32_t pins)
 *        {
 *            atomic_inc(&pressioni);
 *            k_sem_give(&sem_pressione);
 *        }
 * 4. In main(), dopo la configurazione dei pin:
 *        static struct gpio_callback cb_data;
 *        gpio_pin_interrupt_configure_dt(&btn, GPIO_INT_EDGE_TO_ACTIVE);
 *        gpio_init_callback(&cb_data, pulsante_premuto, BIT(btn.pin));
 *        gpio_add_callback(btn.port, &cb_data);
 *    Controlla i valori restituiti (0 = ok). Poi stampa "Pronto: premi sw0".
 * 5. Ciclo infinito nel main:
 *        k_sem_take(&sem_pressione, K_FOREVER);   il thread DORME finche' non premi
 *        gpio_pin_toggle_dt(&led);
 *        printk("Pressioni: %d\n", (int)atomic_get(&pressioni));
 * 6. Esegui e nel monitor di Renode premi e rilascia piu' volte:
 *        sysbus.gpioPortA.sw0 Press
 *        sysbus.gpioPortA.sw0 Release
 *
 * OUTPUT ATTESO (tre pressioni)
 *     Pronto: premi sw0
 *     Pressioni: 1          (LED acceso)
 *     Pressioni: 2          (LED spento)
 *     Pressioni: 3          (LED acceso)
 * Il rilascio NON conta: l'interrupt e' solo sul fronte "verso attivo".
 *
 * CONCETTI NUOVI
 * - Interrupt GPIO sull'STM32: il pin PA0 e' collegato alla linea 0 dell'EXTI
 *   (External Interrupt controller), che avvisa il NVIC (il controller degli interrupt del
 *   Cortex-M). Il driver di Zephyr configura EXTI e NVIC per te; tu scrivi solo la callback.
 * - GPIO_INT_EDGE_TO_ACTIVE: interrupt sul fronte in cui il pin diventa "attivo"
 *   (premuto), qualunque sia il livello elettrico.
 * - gpio_callback, gpio_init_callback, gpio_add_callback: la registrazione della callback.
 *   BIT(btn.pin) e' la maschera dei pin che interessano questa callback.
 * - Contesto di interrupt: nella callback si fa il minimo (contare, k_sem_give); il lavoro
 *   vero (LED, printk) lo fa il thread, che intanto dorme senza consumare CPU.
 * - atomic_t: un intero letto e scritto in modo indivisibile da interrupt e thread.
 *
 * COME VERIFICARE DA SOLO
 *     .\verifica.ps1 modulo6\02_pulsante_interrupt
 * La verifica preme sw0 tre volte e controlla messaggi e LED.
 *
 * ERRORI TIPICI
 * - Dimenticare gpio_pin_interrupt_configure_dt: la callback non viene mai chiamata.
 * - struct gpio_callback come variabile locale di una funzione che poi termina: il
 *   driver conserva il suo indirizzo e la usa dopo -> crash. Va static (o globale).
 * - BIT(btn.pin) sbagliato (per esempio BIT(1)): la callback non scatta per quel pin.
 * - k_msleep o printk lunghi nella callback: in interrupt non si dorme (vedi Modulo 5).
 * - GPIO_INT_EDGE_BOTH: conterebbe anche i rilasci.
 *
 * BONUS (facoltativo)
 * - Aggiungi il pulsante extra sw1 su PE7 (overlay gpio-keys, CORSO.md sezione 3) con
 *   la SUA callback: PA0 e PE7 sono su linee EXTI diverse (0 e 7). Cosa succederebbe con
 *   due pulsanti su PA7 e PE7? (Teoria sezione 4: le linee EXTI sono condivise tra le
 *   porte, il registro SYSCFG_EXTICR ne sceglie una sola.)
 * - Alternativa senza interrupt: un k_timer ogni 10 ms che legge il pin e riconosce il
 *   fronte confrontando con la lettura precedente (teoria sezione 4.1). Scrivila in una
 *   seconda versione e confronta: latenza, consumo, semplicita'.
 * - Misura quanto tempo passa tra la pressione e la stampa: salva k_cycle_get_32() nella
 *   callback e confrontalo nel thread.
 *
 * Scrivi il tuo codice qui sotto.
 */

#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/atomic.h>
#include <zephyr/sys/printk.h>

/* TODO: gpio_dt_spec, semaforo, contatore e callback (passi 1-3) */

int main(void)
{
	/* TODO: configurazione, registrazione della callback e ciclo (passi 1, 4, 5) */

	return 0;
}
