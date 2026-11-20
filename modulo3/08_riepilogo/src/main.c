/*
 * Esercizio 8 (Modulo 3) - Riepilogo: quattro LED, quattro thread
 *
 * Teoria: teoria/04_thread_scheduling.md (tutto) e teoria/03, sezione 7 (GPIO).
 *
 * OBIETTIVO
 * Far lampeggiare i 4 LED della scheda a periodi diversi, ognuno con il SUO thread
 * (stessa funzione, parametri diversi), e un thread di controllo che ogni 2 secondi
 * stampa quante volte ha cambiato stato ciascun LED.
 *
 * PASSI
 * 1. Gia' pronti qui sotto: l'array "leds" con le 4 gpio_dt_spec, l'array dei periodi
 *    (250, 500, 750, 1000 ms) e l'array "cambi" dei contatori.
 * 2. Scrivi  void lampeggia(void *p1, void *p2, void *p3)  che:
 *      - ricava l'indice del LED: int i = (int)(intptr_t)p1;
 *      - configura leds[i] come uscita spenta (controlla gpio_is_ready_dt);
 *      - in ciclo infinito: gpio_pin_toggle_dt(&leds[i]); cambi[i]++;
 *        k_msleep(periodi_ms[i]);
 * 3. Crea 4 thread con K_THREAD_DEFINE (led_0 ... led_3), stack 512, priorita' 6, che
 *    usano lampeggia con p1 = (void *)0, (void *)1, (void *)2, (void *)3.
 * 4. Scrivi il thread "controllo" (priorita' 4, quindi piu' importante dei LED) che:
 *      - dorme 2000 ms;
 *      - stampa "stato: L0=%u L1=%u L2=%u L3=%u" con i 4 contatori;
 *      - la PRIMA volta stampa anche "L0 piu' veloce di L3: si" se cambi[0] > cambi[3],
 *        altrimenti "... : no" (usa una variabile "prima_volta");
 *      - ricomincia.
 * 5. In main() stampa "Avvio 4 thread LED" e ritorna.
 * 6. Esegui. In Renode guarda i LED con  sysbus.gpio0.led0 State ... led3 State.
 *    Dopo 2 secondi L0 dovrebbe aver cambiato stato circa 8 volte, L3 circa 2.
 * 7. Lancia la verifica.
 *
 * OUTPUT ATTESO (numeri indicativi)
 *     Avvio 4 thread LED
 *     stato: L0=8 L1=4 L2=3 L3=2
 *     L0 piu' veloce di L3: si
 *     stato: L0=16 L1=8 L2=6 L3=4
 *     ...
 *
 * CONCETTI NUOVI
 * - Nessuno nuovo: thread con parametri, priorita', sleep, GPIO dal devicetree.
 * - Osserva pero': i contatori "cambi" sono scritti dai thread LED e letti dal
 *   controllo. Qui va bene (un solo scrittore per contatore, lettura di un intero a
 *   32 bit). Con piu' scrittori servira' un mutex (Modulo 4).
 *
 * COME VERIFICARE DA SOLO
 *     .\verifica.ps1 modulo3\08_riepilogo
 *
 * ERRORI TIPICI
 * - Stack troppo piccolo (es. 256) per un thread che chiama i driver GPIO.
 * - Dimenticare di incrementare cambi[i]: tutti i contatori a 0.
 * - Thread di controllo con priorita' piu' bassa dei LED: funziona lo stesso (i LED
 *   dormono quasi sempre), ma pensa a cosa succederebbe se un LED non dormisse mai.
 *
 * BONUS (facoltativo)
 * - Aggiungi CONFIG_THREAD_ANALYZER=y e CONFIG_THREAD_ANALYZER_USE_PRINTK=y e fai
 *   stampare al controllo anche thread_analyzer_print(0): quanto stack usano davvero
 *   i thread LED? Riduci lo stack di conseguenza.
 * - Fai sospendere al controllo il thread led_3 dopo 3 stampe e riprenderlo dopo 6.
 *
 * Scrivi il tuo codice qui sotto.
 */

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/drivers/gpio.h>

#define N_LED 4

static const struct gpio_dt_spec leds[N_LED] = {
	GPIO_DT_SPEC_GET(DT_ALIAS(led0), gpios),
	GPIO_DT_SPEC_GET(DT_ALIAS(led1), gpios),
	GPIO_DT_SPEC_GET(DT_ALIAS(led2), gpios),
	GPIO_DT_SPEC_GET(DT_ALIAS(led3), gpios),
};

static const int periodi_ms[N_LED] = {250, 500, 750, 1000};

static volatile uint32_t cambi[N_LED];

/* TODO: passi 2, 3 e 4 */

int main(void)
{
	/* TODO: passo 5 */

	return 0;
}
