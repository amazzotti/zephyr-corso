/*
 * Esercizio 5 (Modulo 4) - Coda di messaggi
 *
 * Teoria: teoria/05_sincronizzazione.md, sezione 6.
 *
 * OBIETTIVO
 * Il produttore genera una "misura" ogni 200 ms e la mette in una coda; il consumatore
 * le preleva e le stampa. Prima il consumatore e' veloce e la coda resta quasi vuota;
 * dalla misura 10 in poi diventa lento (500 ms per misura): la coda si riempie e il
 * produttore deve decidere cosa fare delle misure in eccesso.
 *
 * PASSI
 * 1. La struct e la coda sono gia' definite:
 *        struct misura { uint32_t n; int32_t valore; uint32_t tempo_ms; };
 *        K_MSGQ_DEFINE(coda, sizeof(struct misura), DIM_CODA, 4);
 * 2. produttore(): per n da 1 a N_MISURE
 *    - riempi una struct misura: n, valore = 200 + (n % 7) (decimi di grado: 20.0 C...),
 *      tempo_ms = k_uptime_get_32()
 *    - k_msgq_put(&coda, &m, K_NO_WAIT);
 *      se restituisce un valore diverso da 0 la coda e' piena: incrementa perse e stampa
 *          "Coda piena: misura %u persa"
 *    - k_msleep(200);
 *    Alla fine stampa "Produttore: finito, perse %u misure".
 * 3. consumatore(): ciclo infinito
 *    - k_msgq_get(&coda, &m, K_FOREVER);
 *    - stampa "Misura n=%u valore=%d.%d C (in coda: %u)" con valore / 10, valore % 10
 *      e k_msgq_num_used_get(&coda)
 *    - se m.n >= 10: k_msleep(500)  (da qui in poi il consumatore e' lento)
 *
 * OUTPUT ATTESO
 *     Misura n=1 valore=20.1 C (in coda: 0)
 *     ...
 *     Misura n=10 valore=20.3 C (in coda: 0)
 *     ...                                 (in coda cresce: 1, 2, 3...)
 *     Coda piena: misura 25 persa         (il numero dipende dai tempi)
 *     ...
 *     Produttore: finito, perse 11 misure (circa)
 *
 * CONCETTI NUOVI
 * - k_msgq: coda di messaggi di dimensione fissa. put e get COPIANO il messaggio:
 *   dopo il put puoi riusare la variabile m.
 * - DIM_CODA: quanti messaggi puo' contenere al massimo (8 qui).
 * - Coda piena: con K_NO_WAIT il put fallisce subito (-ENOMSG). Con un timeout il
 *   produttore si bloccherebbe ad aspettare spazio: per un sensore questo vorrebbe dire
 *   saltare le letture, non sempre accettabile. E' una scelta di progetto.
 * - Il consumatore bloccato in get(K_FOREVER) non consuma CPU (come con il semaforo).
 *
 * COME VERIFICARE DA SOLO
 *     .\verifica.ps1 modulo4\05_msgq
 *
 * ERRORI TIPICI
 * - Passare m invece di &m a put/get: errore di compilazione (serve un puntatore).
 * - Dimensione nella K_MSGQ_DEFINE diversa da sizeof(struct misura): dati corrotti.
 * - %d con un uint32_t: il compilatore avvisa (warning). Usa %u per gli unsigned.
 * - Stampare valore / 10.0: printk non supporta i float di default; usa la divisione
 *   intera e il resto, come indicato.
 *
 * BONUS (facoltativo)
 * - Invece di scartare la misura nuova, scarta la piu' vecchia: se il put fallisce,
 *   fai un k_msgq_get(&coda, &vecchia, K_NO_WAIT) e riprova il put. Quale delle due
 *   strategie preferiresti per un sensore di temperatura? E per un registro di allarmi?
 * - Calcola nel consumatore il ritardo di ogni misura: k_uptime_get_32() - m.tempo_ms.
 *
 * Scrivi il tuo codice qui sotto.
 */

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

#define N_MISURE  40
#define DIM_CODA  8
#define DIM_STACK 1024

struct misura {
	uint32_t n;         /* numero progressivo */
	int32_t valore;     /* temperatura in decimi di grado (215 = 21.5 C) */
	uint32_t tempo_ms;  /* istante della misura */
};

K_MSGQ_DEFINE(coda, sizeof(struct misura), DIM_CODA, 4);

static uint32_t perse;

static void produttore(void *p1, void *p2, void *p3)
{
	ARG_UNUSED(p1);
	ARG_UNUSED(p2);
	ARG_UNUSED(p3);

	/* TODO: N_MISURE misure, una ogni 200 ms, con k_msgq_put(..., K_NO_WAIT) */
}

static void consumatore(void *p1, void *p2, void *p3)
{
	ARG_UNUSED(p1);
	ARG_UNUSED(p2);
	ARG_UNUSED(p3);

	/* TODO: ciclo infinito con k_msgq_get(..., K_FOREVER), stampa, lento da n >= 10 */
}

K_THREAD_DEFINE(t_produttore, DIM_STACK, produttore, NULL, NULL, NULL, 5, 0, 0);
K_THREAD_DEFINE(t_consumatore, DIM_STACK, consumatore, NULL, NULL, NULL, 6, 0, 0);

int main(void)
{
	printk("Coda di messaggi: %d posti\n", DIM_CODA);
	return 0;
}
