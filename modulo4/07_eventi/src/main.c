/*
 * Esercizio 7 (Modulo 4) - Eventi
 *
 * Teoria: teoria/05_sincronizzazione.md, sezione 8.
 *
 * OBIETTIVO
 * All'avvio di un dispositivo piu' parti si inizializzano in parallelo, con tempi diversi:
 * il sensore (300 ms), la "rete" (800 ms). Il thread principale deve partire solo quando
 * ENTRAMBE sono pronte. Poi deve reagire a un comando (pulsante o timeout), qualunque
 * arrivi per primo. Con gli eventi si aspetta una combinazione di condizioni.
 *
 * PASSI
 * 1. Gia' pronti: l'oggetto K_EVENT_DEFINE(eventi) e i bit EV_SENSORE, EV_RETE,
 *    EV_COMANDO, EV_ERRORE.
 * 2. init_sensore(): k_msleep(300), stampa "Sensore pronto", k_event_post(&eventi, EV_SENSORE).
 *    init_rete():    k_msleep(800), stampa "Rete pronta",    k_event_post(&eventi, EV_RETE).
 *    comando():      k_msleep(2000), stampa "Comando inviato", k_event_post(&eventi, EV_COMANDO).
 * 3. In main():
 *    a. aspetta TUTTI e due i bit:
 *           uint32_t ev = k_event_wait_all(&eventi, EV_SENSORE | EV_RETE, false, K_SECONDS(5));
 *       se ev == 0 stampa "Timeout di avvio" e termina; altrimenti stampa
 *           "Sistema pronto (eventi 0x%x) dopo %lld ms"   con k_uptime_get().
 *    b. aspetta ALMENO UNO tra EV_COMANDO e EV_ERRORE:
 *           ev = k_event_wait(&eventi, EV_COMANDO | EV_ERRORE, false, K_SECONDS(5));
 *       e stampa quale e' arrivato: "Evento ricevuto: comando" oppure "... errore".
 *
 * OUTPUT ATTESO
 *     Attendo sensore e rete...
 *     Sensore pronto
 *     Rete pronta
 *     Sistema pronto (eventi 0x3) dopo 800 ms
 *     Comando inviato
 *     Evento ricevuto: comando
 *
 * CONCETTI NUOVI
 * - k_event: 32 bit, ognuno e' una condizione. post accende dei bit lasciando gli altri.
 * - k_event_wait_all: ritorna quando TUTTI i bit richiesti sono accesi.
 *   k_event_wait: quando ALMENO UNO e' acceso. Valore restituito: i bit accesi
 *   (0 = timeout).
 * - BIT(n) = 1 << n, la macro di Zephyr del Modulo 1. 0x3 = bit 0 e bit 1.
 * - Serve CONFIG_EVENTS=y (gia' nel prj.conf).
 *
 * COME VERIFICARE DA SOLO
 *     .\verifica.ps1 modulo4\07_eventi
 *
 * ERRORI TIPICI
 * - Usare k_event_set invece di k_event_post: set SOSTITUISCE tutti i bit, quindi
 *   "Rete pronta" cancellerebbe "Sensore pronto" e wait_all non ritornerebbe mai.
 * - Confondere wait e wait_all: con wait, main ripartirebbe dopo 300 ms (solo sensore).
 * - Controllare ev == EV_COMANDO invece di (ev & EV_COMANDO): se sono accesi anche
 *   altri bit (sensore, rete) il confronto con == fallisce.
 *
 * BONUS (facoltativo)
 * - Fai fallire l'avvio: init_rete aspetta 6000 ms. Che cosa stampa main?
 * - Fai inviare a un quarto thread EV_ERRORE dopo 1500 ms: quale evento vede main?
 * - Rifai il punto 3a con due semafori: quante righe servono? E se le condizioni fossero 5?
 *
 * Scrivi il tuo codice qui sotto.
 */

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

#define DIM_STACK 1024

#define EV_SENSORE BIT(0)
#define EV_RETE    BIT(1)
#define EV_COMANDO BIT(2)
#define EV_ERRORE  BIT(3)

K_EVENT_DEFINE(eventi);

static void init_sensore(void *p1, void *p2, void *p3)
{
	ARG_UNUSED(p1);
	ARG_UNUSED(p2);
	ARG_UNUSED(p3);

	/* TODO: dormi 300 ms, stampa, post di EV_SENSORE */
}

static void init_rete(void *p1, void *p2, void *p3)
{
	ARG_UNUSED(p1);
	ARG_UNUSED(p2);
	ARG_UNUSED(p3);

	/* TODO: dormi 800 ms, stampa, post di EV_RETE */
}

static void comando(void *p1, void *p2, void *p3)
{
	ARG_UNUSED(p1);
	ARG_UNUSED(p2);
	ARG_UNUSED(p3);

	/* TODO: dormi 2000 ms, stampa, post di EV_COMANDO */
}

K_THREAD_DEFINE(t_sensore, DIM_STACK, init_sensore, NULL, NULL, NULL, 5, 0, 0);
K_THREAD_DEFINE(t_rete, DIM_STACK, init_rete, NULL, NULL, NULL, 5, 0, 0);
K_THREAD_DEFINE(t_comando, DIM_STACK, comando, NULL, NULL, NULL, 5, 0, 0);

int main(void)
{
	printk("Attendo sensore e rete...\n");

	/* TODO: 3a - k_event_wait_all su EV_SENSORE | EV_RETE */

	/* TODO: 3b - k_event_wait su EV_COMANDO | EV_ERRORE */

	return 0;
}
