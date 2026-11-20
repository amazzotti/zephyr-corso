/*
 * Esercizio 4 (Modulo 4) - Semaforo contatore
 *
 * Teoria: teoria/05_sincronizzazione.md, sezioni 4 e 5.
 *
 * OBIETTIVO
 * Ci sono 3 "risorse" uguali (immagina 3 buffer di trasmissione) e 5 thread che le
 * vogliono usare. Un semaforo contatore garantisce che non piu' di 3 thread alla volta
 * stiano usando una risorsa; gli altri aspettano il loro turno.
 *
 * PASSI
 * 1. Il semaforo e' gia' definito: K_SEM_DEFINE(risorse, N_RISORSE, N_RISORSE);
 *    (parte "pieno": 3 risorse libere su 3).
 * 2. Scrivi lavoratore(): il parametro p1 e' il numero del lavoratore (1..5), lo ottieni
 *    con  int id = (int)(intptr_t)p1;
 *    Ripeti GIRI volte:
 *    a. k_sem_take(&risorse, K_FOREVER);
 *    b. aggiorna in_uso (+1) e massimo_in_uso, PROTETTI dal mutex stato (esercizio 2),
 *       e stampa  "Lavoratore %d: prendo una risorsa (in uso: %d)"
 *    c. k_msleep(100 + 50 * id);   (usa la risorsa per un po')
 *    d. in_uso (-1) protetto dal mutex, stampa "Lavoratore %d: libero la risorsa"
 *    e. k_sem_give(&risorse);
 * 3. I 5 thread sono gia' definiti con K_THREAD_DEFINE e passano l'id come p1.
 * 4. main(): aspetta i 5 thread con k_thread_join e stampa
 *        "Fine. Massimo contemporaneo: %d"
 *
 * OUTPUT ATTESO (l'ordine preciso puo' variare)
 *     Lavoratore 1: prendo una risorsa (in uso: 1)
 *     Lavoratore 2: prendo una risorsa (in uso: 2)
 *     Lavoratore 3: prendo una risorsa (in uso: 3)
 *     Lavoratore 1: libero la risorsa
 *     Lavoratore 4: prendo una risorsa (in uso: 3)
 *     ...
 *     Fine. Massimo contemporaneo: 3
 *
 * CONCETTI NUOVI
 * - Semaforo contatore: il valore e' il numero di risorse libere.
 * - Passare un intero come parametro void* a un thread: (void *)1 all'andata,
 *   (int)(intptr_t)p1 al ritorno. Non e' un indirizzo vero, e' un "trucco" comune in C.
 * - Il semaforo limita QUANTI usano le risorse, ma non protegge i dati condivisi
 *   (in_uso): per quelli serve comunque il mutex.
 *
 * COME VERIFICARE DA SOLO
 *     .\verifica.ps1 modulo4\04_semaforo_contatore
 * Controlla che il massimo sia 3 e che "in uso" non superi mai 3.
 *
 * ERRORI TIPICI
 * - k_sem_give dimenticato in un ramo: le risorse "spariscono" e alla fine tutti aspettano.
 * - Aggiornare in_uso DOPO la stampa: la stampa mostra un valore vecchio.
 * - Semaforo con massimo 1: diventa un mutex "povero", lavora un thread alla volta.
 *
 * BONUS (facoltativo)
 * - Stampa anche k_sem_count_get(&risorse) dopo il take: quanto vale? E' sempre
 *   N_RISORSE - in_uso? (Attenzione: viene letto in un momento diverso.)
 * - Usa k_sem_take(&risorse, K_MSEC(50)) e, se scade, stampa
 *   "Lavoratore N: nessuna risorsa, riprovo" e riprova.
 *
 * Scrivi il tuo codice qui sotto.
 */

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

#define N_RISORSE 3
#define GIRI      3
#define DIM_STACK 1024
#define PRIORITA  5

K_SEM_DEFINE(risorse, N_RISORSE, N_RISORSE);
K_MUTEX_DEFINE(stato);

static int in_uso;          /* risorse in uso adesso (protetto da stato) */
static int massimo_in_uso;  /* massimo osservato (protetto da stato) */

static void lavoratore(void *p1, void *p2, void *p3)
{
	int id = (int)(intptr_t)p1;

	ARG_UNUSED(p2);
	ARG_UNUSED(p3);
	ARG_UNUSED(id);   /* TODO: togli questa riga quando usi id */

	/* TODO: GIRI volte: take, aggiorna e stampa, usa la risorsa, libera, give */
}

K_THREAD_DEFINE(lav1, DIM_STACK, lavoratore, (void *)1, NULL, NULL, PRIORITA, 0, 0);
K_THREAD_DEFINE(lav2, DIM_STACK, lavoratore, (void *)2, NULL, NULL, PRIORITA, 0, 0);
K_THREAD_DEFINE(lav3, DIM_STACK, lavoratore, (void *)3, NULL, NULL, PRIORITA, 0, 0);
K_THREAD_DEFINE(lav4, DIM_STACK, lavoratore, (void *)4, NULL, NULL, PRIORITA, 0, 0);
K_THREAD_DEFINE(lav5, DIM_STACK, lavoratore, (void *)5, NULL, NULL, PRIORITA, 0, 0);

int main(void)
{
	printk("Pool di %d risorse, 5 lavoratori\n", N_RISORSE);

	/* TODO: k_thread_join dei 5 lavoratori, poi stampa il massimo contemporaneo */

	return 0;
}
