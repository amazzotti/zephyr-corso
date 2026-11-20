/*
 * Esercizio 7 (Modulo 5) - Riepilogo: battito e supervisore
 *
 * Teoria: teoria/06_tempo_lavoro_differito.md, sezioni 4, 6 e 7;
 *         teoria/03_build_kconfig_devicetree.md per i LED.
 *
 * OBIETTIVO
 * Due funzioni tipiche di un firmware vero:
 * - un LED di "battito" (heartbeat): led0 cambia stato ogni 500 ms, comandato da un
 *   k_timer. Se smette di lampeggiare, il firmware e' bloccato.
 * - un SUPERVISORE: due thread di lavoro ("acquisizione" e "comunicazione") devono dare
 *   un segno di vita almeno ogni 3 secondi. Se uno smette, il supervisore stampa quale
 *   e accende led1 (allarme).
 * Il thread "comunicazione" dopo 5 cicli si blocca apposta.
 *
 * PASSI
 * 1. Battito: in main configura led_battito e led_allarme come uscite spente
 *    (gpio_is_ready_dt, gpio_pin_configure_dt con GPIO_OUTPUT_INACTIVE, Modulo 2).
 *    Nella callback battito() fai gpio_pin_toggle_dt(&led_battito) (si puo' fare da
 *    interrupt). Avvia il timer: 500 ms, periodo 500 ms.
 * 2. Supervisore: la struct sorvegliato e' gia' pronta, contiene un k_work_delayable e
 *    il nome del thread. Ci sono due variabili: sorv_acq e sorv_com.
 *    - In main: k_work_init_delayable(&sorv_acq.lavoro, scaduto) (idem per sorv_com),
 *      poi k_work_schedule di entrambi con K_SECONDS(TIMEOUT_S).
 *    - scaduto(struct k_work *w): ricava il sorvegliato con
 *          struct k_work_delayable *d = k_work_delayable_from_work(w);
 *          struct sorvegliato *s = CONTAINER_OF(d, struct sorvegliato, lavoro);
 *      stampa "SUPERVISORE: thread '%s' bloccato" con s->nome e accendi led_allarme.
 * 3. segno_di_vita(struct sorvegliato *s): k_work_reschedule(&s->lavoro, K_SECONDS(TIMEOUT_S));
 * 4. acquisizione(): ciclo infinito: k_msleep(1000); stampa "Acquisizione: ciclo %u";
 *    segno_di_vita(&sorv_acq);
 *    comunicazione(): come sopra con "Comunicazione: ciclo %u" e sorv_com, ma al ciclo 5,
 *    dopo il segno di vita, stampa "Comunicazione: mi blocco (simulato)" e
 *    k_sleep(K_FOREVER).
 *
 * OUTPUT ATTESO
 *     Battito e supervisore avviati
 *     Acquisizione: ciclo 1
 *     Comunicazione: ciclo 1
 *     ...
 *     Comunicazione: ciclo 5
 *     Comunicazione: mi blocco (simulato)
 *     Acquisizione: ciclo 6
 *     Acquisizione: ciclo 7
 *     SUPERVISORE: thread 'comunicazione' bloccato   (3 s dopo l'ultimo segno di vita:
 *     Acquisizione: ciclo 8                           puo' comparire prima o dopo il ciclo 8)
 *     ...
 * In Renode: led0 cambia stato ogni 500 ms (prova "sysbus.gpio0.led0 State" piu' volte),
 * led1 si accende con l'allarme.
 *
 * CONCETTI NUOVI
 * - CONTAINER_OF(puntatore_al_campo, tipo_struct, nome_campo): dal puntatore a un campo
 *   risale all'inizio della struct che lo contiene. Cosi' UNA sola funzione scaduto()
 *   serve per tutti i thread sorvegliati: e' il modo standard di passare dati a un k_work.
 * - k_work_delayable_from_work: dal k_work ricevuto dall'handler al k_work_delayable.
 * - Heartbeat e supervisore: strumenti di diagnosi che vedrai di nuovo con il watchdog
 *   hardware (Modulo 6) e nel progetto finale.
 *
 * COME VERIFICARE DA SOLO
 *     .\verifica.ps1 modulo5\07_riepilogo
 * Controlla l'allarme sul thread giusto (e mai sull'altro) e che led1 sia acceso.
 *
 * ERRORI TIPICI
 * - Usare k_work_schedule in segno_di_vita: la scadenza non si sposta e il supervisore
 *   segnala anche il thread sano.
 * - CONTAINER_OF con il nome di campo sbagliato: errore di compilazione poco leggibile.
 * - Configurare i LED dopo aver avviato il timer: la prima callback usa un pin non
 *   configurato.
 *
 * BONUS (facoltativo)
 * - Quando l'allarme scatta, il battito diventa veloce (100 ms): k_timer_start di nuovo
 *   con il nuovo periodo, da scaduto().
 * - Aggiungi un terzo thread sorvegliato: quante righe di codice servono? (Ecco perche'
 *   CONTAINER_OF.)
 *
 * Scrivi il tuo codice qui sotto.
 */

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/drivers/gpio.h>

#define TIMEOUT_S 3
#define DIM_STACK 1024

static const struct gpio_dt_spec led_battito = GPIO_DT_SPEC_GET(DT_ALIAS(led0), gpios);
static const struct gpio_dt_spec led_allarme = GPIO_DT_SPEC_GET(DT_ALIAS(led1), gpios);

/* --- battito ---------------------------------------------------------------------------- */
static void battito(struct k_timer *t)
{
	ARG_UNUSED(t);
	/* TODO: inverti led_battito */
}

K_TIMER_DEFINE(timer_battito, battito, NULL);

/* --- supervisore ------------------------------------------------------------------------ */
struct sorvegliato {
	struct k_work_delayable lavoro;
	const char *nome;
};

static struct sorvegliato sorv_acq = { .nome = "acquisizione" };
static struct sorvegliato sorv_com = { .nome = "comunicazione" };

static void scaduto(struct k_work *w)
{
	ARG_UNUSED(w);
	/* TODO: CONTAINER_OF, stampa, accendi led_allarme */
}

static void segno_di_vita(struct sorvegliato *s)
{
	ARG_UNUSED(s);
	/* TODO: k_work_reschedule */
}

/* --- thread di lavoro ------------------------------------------------------------------ */
static void acquisizione(void *p1, void *p2, void *p3)
{
	ARG_UNUSED(p1);
	ARG_UNUSED(p2);
	ARG_UNUSED(p3);

	/* TODO */
}

static void comunicazione(void *p1, void *p2, void *p3)
{
	ARG_UNUSED(p1);
	ARG_UNUSED(p2);
	ARG_UNUSED(p3);

	/* TODO */
}

/* Partono dopo 100 ms: main ha il tempo di inizializzare LED e supervisore. */
K_THREAD_DEFINE(t_acq, DIM_STACK, acquisizione, NULL, NULL, NULL, 5, 0, 100);
K_THREAD_DEFINE(t_com, DIM_STACK, comunicazione, NULL, NULL, NULL, 5, 0, 100);

int main(void)
{
	/* TODO: configura i LED, avvia il battito, inizializza e avvia il supervisore */

	printk("Battito e supervisore avviati\n");

	/* Togli queste righe quando usi davvero gli oggetti. */
	ARG_UNUSED(led_battito);
	ARG_UNUSED(led_allarme);
	ARG_UNUSED(sorv_acq);
	ARG_UNUSED(sorv_com);
	ARG_UNUSED(scaduto);
	ARG_UNUSED(segno_di_vita);

	return 0;
}
