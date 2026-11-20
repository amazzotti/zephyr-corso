/*
 * Esercizio 1 (Modulo 5) - Timer del kernel
 *
 * Teoria: teoria/06_tempo_lavoro_differito.md, sezioni 1, 2 e 4.
 *
 * OBIETTIVO
 * Usare k_timer in due modi: periodico (scade ogni 500 ms e conta le scadenze) e
 * one-shot (scade una volta sola dopo 1500 ms). Il main legge ogni secondo quante
 * volte e' scaduto il periodico, poi lo ferma dopo 5 secondi.
 *
 * PASSI
 * 1. Completa le tre callback (gia' collegate ai timer con K_TIMER_DEFINE):
 *    - periodico_scaduto(): scadenze++;
 *    - periodico_fermato(): printk("Timer fermato dopo %u scadenze\n", scadenze);
 *    - oneshot_scaduto():   printk("One-shot scaduto a %lld ms\n", k_uptime_get());
 * 2. In main():
 *    a. avvia il periodico: prima scadenza dopo 250 ms, poi ogni 500 ms
 *           k_timer_start(&t_periodico, K_MSEC(250), K_MSEC(500));
 *    b. avvia il one-shot: dopo 1500 ms, periodo K_NO_WAIT (= nessuna ripetizione)
 *    c. per i da 1 a 5: k_msleep(1000); poi stampa
 *           "Secondo %d: scadenze=%u, status=%u"
 *       dove status = k_timer_status_get(&t_periodico)  (scadenze dall'ultima lettura)
 *    d. k_timer_stop(&t_periodico);
 *
 * OUTPUT ATTESO
 *     Avvio dei timer
 *     Secondo 1: scadenze=2, status=2
 *     One-shot scaduto a 1500 ms
 *     Secondo 2: scadenze=4, status=2
 *     ...
 *     Secondo 5: scadenze=10, status=2
 *     Timer fermato dopo 10 scadenze
 *
 * CONCETTI NUOVI
 * - K_TIMER_DEFINE(nome, funzione_scadenza, funzione_stop): un timer statico.
 * - k_timer_start(&t, prima_scadenza, periodo); periodo K_NO_WAIT = one-shot.
 * - La funzione di scadenza gira in CONTESTO DI INTERRUPT (la chiama la ISR del timer
 *   di sistema, RTC1 sul nRF52840): deve essere brevissima e non puo' bloccarsi.
 *   Lo approfondirai nell'esercizio 2.
 * - k_timer_status_get: quante scadenze dall'ultima chiamata (e azzera il conteggio).
 * - Perche' la prima scadenza a 250 ms e non a 500? Cosi' le scadenze (250, 750, 1250...)
 *   non coincidono MAI con i risvegli del main (1000, 2000...): niente ambiguita' su chi
 *   arriva prima.
 *
 * COME VERIFICARE DA SOLO
 *     .\verifica.ps1 modulo5\01_k_timer
 *
 * ERRORI TIPICI
 * - scadenze modificata nella ISR e letta nel main: va dichiarata volatile (Modulo 1),
 *   altrimenti il compilatore puo' tenere in un registro un valore vecchio.
 * - Usare K_MSEC(0) come periodo pensando a "nessun periodo": usa K_NO_WAIT.
 * - Lunghe elaborazioni o printk a raffica nella callback: rallentano tutto il sistema.
 *
 * BONUS (facoltativo)
 * - Usa k_timer_user_data_set(&t_periodico, &qualcosa) per passare un puntatore alla
 *   callback (e leggilo con k_timer_user_data_get(t)).
 * - Dopo lo stop, riavvia il periodico con periodo 100 ms per 1 secondo: quante scadenze?
 * - k_timer_remaining_get(&t): quanto manca alla prossima scadenza? Stampalo.
 *
 * Scrivi il tuo codice qui sotto.
 */

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

/* Modificata dalla callback (contesto di interrupt), letta dal main. */
static volatile uint32_t scadenze;

static void periodico_scaduto(struct k_timer *t)
{
	ARG_UNUSED(t);
	/* TODO */
}

static void periodico_fermato(struct k_timer *t)
{
	ARG_UNUSED(t);
	/* TODO */
}

static void oneshot_scaduto(struct k_timer *t)
{
	ARG_UNUSED(t);
	/* TODO */
}

K_TIMER_DEFINE(t_periodico, periodico_scaduto, periodico_fermato);
K_TIMER_DEFINE(t_oneshot, oneshot_scaduto, NULL);

int main(void)
{
	printk("Avvio dei timer\n");

	/* TODO: avvia i timer, ciclo di 5 secondi con le stampe, ferma il periodico */

	return 0;
}
