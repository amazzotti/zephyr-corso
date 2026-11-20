/*
 * Esercizio 6 (Modulo 4) - FIFO con un pool statico
 *
 * Teoria: teoria/05_sincronizzazione.md, sezione 7.
 *
 * OBIETTIVO
 * Passare da un thread all'altro dei "pacchetti" senza copiarli: si passano i loro
 * indirizzi con una k_fifo. I pacchetti vengono da un pool statico di 4 elementi e
 * tornano al pool quando il consumatore ha finito: niente malloc.
 *
 * PASSI
 * 1. Gia' pronti: struct pacchetto (con il campo riservato fifo_reserved per primo),
 *    l'array pool[DIM_POOL] e due FIFO: liberi e pieni.
 * 2. In main(), PRIMA che i thread partano (partono dopo 100 ms, vedi K_THREAD_DEFINE),
 *    metti tutti gli elementi del pool nella FIFO dei liberi:
 *        for (...) k_fifo_put(&liberi, &pool[i]);
 * 3. produttore(): per n da 1 a N_PACCHETTI
 *    - struct pacchetto *p = k_fifo_get(&liberi, K_FOREVER);   (aspetta un libero)
 *    - riempi p->n = n e p->testo con snprintk(p->testo, sizeof(p->testo), "dato-%u", n)
 *    - k_fifo_put(&pieni, p);
 *    - k_msleep(100);
 * 4. consumatore(): ciclo infinito
 *    - struct pacchetto *p = k_fifo_get(&pieni, K_FOREVER);
 *    - stampa "Pacchetto %u: %s (indirizzo %p)"
 *    - k_msleep(300);   (il consumatore e' piu' lento)
 *    - k_fifo_put(&liberi, p);   (lo restituisce al pool)
 * 5. Osserva gli indirizzi: sono sempre gli stessi 4 che si ripetono.
 *
 * OUTPUT ATTESO
 *     Pool di 4 pacchetti pronto
 *     Pacchetto 1: dato-1 (indirizzo 0x20000a40)     (indirizzi a titolo di esempio)
 *     Pacchetto 2: dato-2 (indirizzo 0x20000a5c)
 *     ...
 *     Pacchetto 5: dato-5 (indirizzo 0x20000a40)     <- di nuovo il primo!
 *     ...
 *     Pacchetto 12: dato-12 (indirizzo ...)
 *
 * CONCETTI NUOVI
 * - k_fifo: passa PUNTATORI, non copie. Il primo campo della struct (void *) e'
 *   riservato al kernel, che lo usa per collegare gli elementi in lista.
 * - Pool statico: un array di elementi preallocati + una FIFO dei liberi. E' come una
 *   malloc "fatta in casa", con dimensione fissa e senza frammentazione.
 * - Contropressione: quando i liberi finiscono il produttore aspetta; il sistema si
 *   adatta da solo alla velocita' del consumatore, senza perdere dati.
 * - Proprieta' del dato: dopo k_fifo_put(&pieni, p) il produttore NON deve piu' toccare p.
 * - %p stampa un indirizzo.
 *
 * COME VERIFICARE DA SOLO
 *     .\verifica.ps1 modulo4\06_fifo
 *
 * ERRORI TIPICI
 * - Mettere fifo_reserved non come primo campo: il kernel sovrascrive i tuoi dati.
 * - Usare una variabile locale del produttore come pacchetto: quando il consumatore la
 *   legge e' gia' stata sovrascritta (o la funzione e' finita). Per questo il pool.
 * - Dimenticare di rimettere il pacchetto nei liberi: dopo 4 pacchetti tutto si ferma.
 * - Mettere un elemento in due FIFO contemporaneamente: la lista si corrompe.
 *
 * BONUS (facoltativo)
 * - Stampa nel produttore quando deve aspettare un pacchetto libero: prova prima
 *   k_fifo_get(&liberi, K_NO_WAIT) e se restituisce NULL stampa
 *   "Produttore: pool esaurito, aspetto" e poi aspetta con K_FOREVER.
 * - Rifai lo stesso esercizio con una k_msgq di 4 elementi: cosa cambia nel codice?
 *   Quando conviene l'una e quando l'altra?
 *
 * Scrivi il tuo codice qui sotto.
 */

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

#define DIM_POOL    4
#define N_PACCHETTI 12
#define DIM_STACK   1024

struct pacchetto {
	void *fifo_reserved;   /* riservato al kernel: deve essere il PRIMO campo */
	uint32_t n;
	char testo[16];
};

static struct pacchetto pool[DIM_POOL];

K_FIFO_DEFINE(liberi);
K_FIFO_DEFINE(pieni);

static void produttore(void *p1, void *p2, void *p3)
{
	ARG_UNUSED(p1);
	ARG_UNUSED(p2);
	ARG_UNUSED(p3);

	/* TODO: N_PACCHETTI volte: prendi un libero, riempilo, mettilo in pieni */
}

static void consumatore(void *p1, void *p2, void *p3)
{
	ARG_UNUSED(p1);
	ARG_UNUSED(p2);
	ARG_UNUSED(p3);

	/* TODO: ciclo infinito: prendi da pieni, stampa, aspetta, restituisci a liberi */
}

/* Partono dopo 100 ms: main ha il tempo di riempire il pool. */
K_THREAD_DEFINE(t_produttore, DIM_STACK, produttore, NULL, NULL, NULL, 5, 0, 100);
K_THREAD_DEFINE(t_consumatore, DIM_STACK, consumatore, NULL, NULL, NULL, 6, 0, 100);

int main(void)
{
	/* TODO: metti tutti gli elementi di pool nella FIFO liberi */
	ARG_UNUSED(pool);   /* TODO: togli questa riga quando usi pool */

	printk("Pool di %d pacchetti pronto\n", DIM_POOL);
	return 0;
}
