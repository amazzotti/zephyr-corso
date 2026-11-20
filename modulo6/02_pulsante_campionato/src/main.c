/*
 * Esercizio 2 (Modulo 6) - Pulsante campionato con un timer
 *
 * Teoria: teoria/07_periferiche_driver.md, sezione 4;
 *         teoria/06_tempo_lavoro_differito.md (k_timer, contesto ISR).
 *
 * OBIETTIVO
 * Contare le pressioni di sw0 senza un ciclo di polling nel thread: un k_timer
 * "guarda" il pulsante ogni 10 ms, riconosce il FRONTE di pressione (prima rilasciato,
 * ora premuto) e lo segnala al thread principale, che dorme finche' non arriva una
 * pressione, poi stampa il totale e inverte led0.
 *
 * +------------------------------------------------------------------------------+
 * | LIMITE DI RENODE: gli interrupt GPIO non scattano nel simulatore (Renode     |
 * | 1.16 + Zephyr 4.5): la callback registrata con gpio_add_callback non viene   |
 * | mai chiamata. Per questo qui CAMPIONIAMO il pulsante con un timer, che e'    |
 * | comunque una tecnica molto usata nei firmware veri (vedi teoria, sezione 4). |
 * | Sulla scheda vera si userebbe l'interrupt: lo trovi nel BONUS.               |
 * +------------------------------------------------------------------------------+
 *
 * PASSI
 * 1. gpio_dt_spec per sw0 e led0 come nell'esercizio 1, con i controlli
 *    gpio_is_ready_dt e la configurazione (GPIO_INPUT, GPIO_OUTPUT_INACTIVE).
 * 2. Variabili globali:
 *        static atomic_t pressioni;                 (contatore condiviso con l'ISR)
 *        K_SEM_DEFINE(sem_pressione, 0, 10);        (segnale dal timer al thread)
 * 3. Scrivi la funzione del timer (gira in contesto di INTERRUPT, come nel Modulo 5):
 *        static void campiona(struct k_timer *t)
 *    Dentro:
 *      - static int precedente = 0;   (lo stato letto al giro prima)
 *      - int ora = gpio_pin_get_dt(&btn);   (1 = premuto, 0 = rilasciato)
 *      - se ora == 1 e precedente == 0: e' un FRONTE di pressione ->
 *            atomic_inc(&pressioni); k_sem_give(&sem_pressione);
 *      - precedente = ora;
 *    Nient'altro: niente printk, niente attese.
 * 4. Definisci il timer e avvialo in main() dopo la configurazione dei pin:
 *        K_TIMER_DEFINE(timer_pulsante, campiona, NULL);
 *        k_timer_start(&timer_pulsante, K_MSEC(10), K_MSEC(10));
 * 5. Stampa "Pronto: premi sw0".
 * 6. Ciclo infinito: k_sem_take(&sem_pressione, K_FOREVER); poi
 *        gpio_pin_toggle_dt(&led);
 *        printk("Pressioni: %d\n", (int)atomic_get(&pressioni));
 * 7. Prova in Renode premendo e rilasciando piu' volte dal monitor
 *    (sysbus.gpio0.sw0 Press, sysbus.gpio0.sw0 Release, ...).
 *
 * OUTPUT ATTESO (dopo tre pressioni)
 *     Pronto: premi sw0
 *     Pressioni: 1
 *     Pressioni: 2
 *     Pressioni: 3
 * Dopo un numero dispari di pressioni led0 e' acceso.
 *
 * CONCETTI NUOVI
 * - Campionamento: leggere un ingresso a intervalli regolari invece di aspettare che
 *   sia lui a "chiamare". Con 10 ms una pressione umana (100 ms o piu') non si perde.
 * - Rilevamento del fronte: confrontare la lettura attuale con la precedente.
 *   Premuto da 5 secondi NON e' 5 secondi di pressioni: conta solo il passaggio 0 -> 1.
 * - Lavoro diviso: la funzione del timer (ISR) fa il minimo, il thread fa il resto.
 * - atomic_t: un intero modificabile dall'ISR e leggibile dal thread senza race
 *   condition (Modulo 4).
 * - Sulla scheda vera: interrupt GPIO (teoria, sezione 4): il pin stesso genera
 *   l'interrupt tramite la periferica GPIOTE del nRF52, senza campionare.
 *
 * COME VERIFICARE DA SOLO
 *     .\verifica.ps1 modulo6\02_pulsante_campionato
 * La verifica fa tre pressioni e controlla il conteggio e il LED.
 *
 * ERRORI TIPICI
 * - Contare "se premuto" invece che "se appena premuto": a ogni giro del timer (ogni
 *   10 ms) il contatore sale finche' tieni premuto.
 * - "precedente" come variabile locale NON static: si azzera a ogni chiamata e il
 *   fronte non si riconosce mai (o sempre).
 * - Chiamare printk o k_msleep nella funzione del timer: siamo in un interrupt.
 * - Avviare il timer prima di aver configurato il pin: le prime letture sono sbagliate.
 * - Periodo troppo lungo (es. 500 ms): una pressione breve cade tra due campioni.
 *
 * BONUS (facoltativo)
 * - Conta anche i rilasci (fronte 1 -> 0) e stampa "Rilasci: N".
 * - Gestisci sw0 e sw1 nella stessa funzione del timer, con due "precedente".
 * - Versione a INTERRUPT, da provare sulla scheda vera (in Renode compila ma la
 *   callback non scatta). Al posto del timer:
 *        static struct gpio_callback cb_pulsante;
 *        static void pulsante_premuto(const struct device *dev,
 *                                     struct gpio_callback *cb, uint32_t pins)
 *        { atomic_inc(&pressioni); k_sem_give(&sem_pressione); }
 *    e in main():
 *        gpio_pin_interrupt_configure_dt(&btn, GPIO_INT_EDGE_TO_ACTIVE);
 *        gpio_init_callback(&cb_pulsante, pulsante_premuto, BIT(btn.pin));
 *        gpio_add_callback(btn.port, &cb_pulsante);
 *    Mettila sotto #if 0 ... #endif o in un file a parte per non perderla.
 *
 * Scrivi il tuo codice qui sotto.
 */

#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/atomic.h>
#include <zephyr/sys/printk.h>

/* TODO: gpio_dt_spec, contatore, semaforo, funzione del timer e timer (passi 1-4) */

int main(void)
{
	/* TODO: configurazione, avvio del timer e ciclo (passi 1, 4-6) */

	return 0;
}
