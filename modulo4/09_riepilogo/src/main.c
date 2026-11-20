/*
 * Esercizio 9 (Modulo 4) - Riepilogo: una pipeline di elaborazione
 *
 * Teoria: teoria/05_sincronizzazione.md, tutto; in particolare sezioni 3, 6 e 12.
 *
 * OBIETTIVO
 * Costruire da solo una piccola pipeline a tre stadi, come in un firmware vero:
 *
 *   [generatore] --coda_grezzi--> [filtro] --coda_filtrati--> [stampa]
 *                                    |
 *                               statistiche (protette da mutex), lette da [stampa]
 *
 * - generatore: ogni 100 ms produce una misura "grezza" di temperatura (in decimi di
 *   grado) con un po' di rumore e, ogni tanto, un picco anomalo.
 * - filtro: calcola la media mobile delle ultime 5 misure e aggiorna le statistiche
 *   (conteggio, minimo e massimo delle misure grezze).
 * - stampa: stampa ogni misura filtrata e, ogni 20 misure, le statistiche.
 *
 * PASSI
 * 1. Gia' pronti: struct grezzo, struct filtrato, le due code, la struct statistiche con
 *    il suo mutex e la funzione genera_valore(n) che simula il sensore.
 * 2. generatore(): per n = 1, 2, 3...: struct grezzo g = { n, genera_valore(n) };
 *    k_msgq_put(&coda_grezzi, &g, K_FOREVER); k_msleep(100);
 * 3. filtro(): ciclo infinito
 *    - k_msgq_get(&coda_grezzi, &g, K_FOREVER);
 *    - metti g.valore in un array finestra[DIM_FINESTRA] usato in modo circolare
 *      (indice = (indice + 1) % DIM_FINESTRA, come il buffer circolare del Modulo 1) e
 *      calcola la media dei valori presenti (all'inizio sono meno di 5!);
 *    - aggiorna stat (campioni, minimo, massimo) DENTRO k_mutex_lock / unlock;
 *    - metti nella coda_filtrati una struct filtrato { n, grezzo, media }.
 * 4. stampa(): ciclo infinito
 *    - k_msgq_get(&coda_filtrati, &f, K_FOREVER);
 *    - stampa "n=%u grezzo=%d media=%d"
 *    - se f.n % 20 == 0: copia stat in una variabile locale DENTRO il mutex, poi FUORI dal
 *      mutex stampa "Statistiche: campioni=%u min=%d max=%d".
 * 5. Scegli le priorita' (generatore la piu' alta, perche'?) e scrivi i tre K_THREAD_DEFINE.
 *
 * OUTPUT ATTESO
 *     Pipeline avviata
 *     n=1 grezzo=218 media=218
 *     n=2 grezzo=205 media=211
 *     n=3 grezzo=210 media=211
 *     ...
 *     n=20 grezzo=... media=...
 *     Statistiche: campioni=20 min=... max=...
 *     ...
 *     Statistiche: campioni=40 min=... max=...
 * Nota il picco (grezzo=... +150) ogni 25 misure: la media lo attenua.
 *
 * CONCETTI NUOVI (nessuno: e' un riepilogo)
 * - Si ripassano: thread e priorita', k_msgq, k_mutex, buffer circolare, struct.
 * - Regola: nel mutex si COPIANO i dati, la stampa (lenta) si fa fuori.
 *
 * COME VERIFICARE DA SOLO
 *     .\verifica.ps1 modulo4\09_riepilogo
 * Verifica le prime tre righe (valori calcolabili a mano) e le statistiche.
 * Controlla a mano: n=1 media=218, n=2 media=(218+205)/2=211 (divisione intera),
 * n=3 media=(218+205+210)/3=211. Con n=20: min=202, max=218. Il picco di n=25 vale 363.
 *
 * ERRORI TIPICI
 * - Media su 5 elementi anche quando ne sono arrivati meno: le prime medie risultano
 *   troppo basse (gli zeri iniziali dell'array contano).
 * - Minimo inizializzato a 0: resta 0 per sempre. Inizializzalo a INT32_MAX (o al primo
 *   valore), il massimo a INT32_MIN.
 * - printk dentro il mutex: funziona, ma blocca il filtro per tutto il tempo della stampa.
 * - Coda troppo piccola + K_NO_WAIT: misure perse senza accorgersene.
 *
 * BONUS (facoltativo)
 * - Il filtro scarta i picchi: se |grezzo - media| > 50 non lo usa nella media e
 *   incrementa stat.scartati (aggiungi il campo). Stampalo nelle statistiche.
 * - Aggiungi un quarto stadio "allarme" che riceve dal filtro solo le medie sopra 220 e
 *   stampa "ALLARME: media %d".
 *
 * Scrivi il tuo codice qui sotto.
 */

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

#define DIM_STACK    1024
#define DIM_CODA     10
#define DIM_FINESTRA 5

struct grezzo {
	uint32_t n;
	int32_t valore;   /* decimi di grado */
};

struct filtrato {
	uint32_t n;
	int32_t grezzo;
	int32_t media;
};

K_MSGQ_DEFINE(coda_grezzi, sizeof(struct grezzo), DIM_CODA, 4);
K_MSGQ_DEFINE(coda_filtrati, sizeof(struct filtrato), DIM_CODA, 4);

struct statistiche {
	uint32_t campioni;
	int32_t minimo;
	int32_t massimo;
};

static struct statistiche stat = {
	.campioni = 0,
	.minimo = INT32_MAX,
	.massimo = INT32_MIN,
};
K_MUTEX_DEFINE(stat_mutex);

/* Simula il sensore: circa 21.0 C con rumore deterministico, picco ogni 25 misure. */
static int32_t genera_valore(uint32_t n)
{
	static const int8_t rumore[] = { 2, 8, -5, 0, 6, -3, 4, -8, 1, 5, -2, -6, 3 };
	int32_t v = 210 + rumore[n % ARRAY_SIZE(rumore)];

	if (n % 25 == 0) {
		v += 150;   /* picco anomalo */
	}
	return v;
}

/* TODO: scrivi generatore(), filtro() e stampa() e i loro K_THREAD_DEFINE */

int main(void)
{
	printk("Pipeline avviata\n");

	/* Togli queste righe quando le usi davvero. */
	ARG_UNUSED(genera_valore);
	ARG_UNUSED(stat);

	return 0;
}
