/*
 * Esercizio 8 (Modulo 4) - Atomic e inversione di priorita'
 *
 * Teoria: teoria/05_sincronizzazione.md, sezioni 9 e 10.
 *
 * OBIETTIVO
 * Parte A: risolvere la race condition dell'esercizio 1 senza mutex, con una variabile
 *          atomica.
 * Parte B: osservare l'inversione di priorita'. Tre thread: ALTA, MEDIA e BASSA
 *          priorita'. ALTA e BASSA condividono una risorsa; MEDIA non la usa ma lavora a
 *          lungo. Si esegue lo stesso scenario due volte: risorsa protetta da un
 *          SEMAFORO (nessuna ereditarieta' di priorita') e da un MUTEX (con
 *          ereditarieta'). Si confronta quanto ha aspettato ALTA.
 *
 * PASSI - Parte A
 * 1. Scrivi incrementa_atomico(): N_INCREMENTI volte  atomic_inc(&contatore); k_yield();
 * 2. In main() crea due thread con k_thread_create (usa le strutture e gli stack gia'
 *    definiti per bassa e media, priorita' 5), fai join e stampa
 *        "Atomic: totale %ld (atteso 20000)"   con atomic_get(&contatore).
 *
 * PASSI - Parte B
 * 3. Completa prendi_risorsa() e libera_risorsa(): se usa_mutex e' true usano
 *    k_mutex_lock/unlock su mutex_risorsa, altrimenti k_sem_take/give su sem_risorsa.
 * 4. Scrivi i tre thread (sequenza nel tempo, in ms dall'inizio dello scenario):
 *    - bassa():  prendi_risorsa(); lavora_ms(300); libera_risorsa();
 *    - alta():   k_msleep(50); t0 = k_uptime_get(); prendi_risorsa();
 *                attesa_alta = k_uptime_get() - t0; libera_risorsa();
 *    - media():  k_msleep(100); lavora_ms(500);     (non tocca la risorsa!)
 *    lavora_ms() e' gia' pronta: tiene occupata la CPU per circa ms millisecondi.
 * 5. Completa esegui_scenario(bool mutex): imposta usa_mutex, crea i tre thread con
 *    k_thread_create e priorita' PRIO_ALTA, PRIO_MEDIA, PRIO_BASSA, fai join di tutti e
 *    tre, stampa
 *        "Scenario semaforo: alta ha atteso %lld ms"   (o "Scenario mutex: ...")
 *    e restituisci attesa_alta.
 * 6. In main(): esegui lo scenario con il semaforo, poi con il mutex, e stampa
 *        "Inversione evitata dal mutex: si"   se l'attesa con mutex e' minore, altrimenti "no".
 * 7. Disegna su carta il diagramma tempo/thread dei due scenari (come nella teoria,
 *    sezione 10) e controlla che i numeri tornino.
 *
 * OUTPUT ATTESO
 *     Parte A
 *     Atomic: totale 20000 (atteso 20000)
 *     Parte B
 *     Scenario semaforo: alta ha atteso 750 ms     (circa)
 *     Scenario mutex: alta ha atteso 250 ms        (circa)
 *     Inversione evitata dal mutex: si
 *
 * CONCETTI NUOVI
 * - atomic_t, ATOMIC_INIT, atomic_inc, atomic_get: lettura-modifica-scrittura
 *   indivisibile, nessun thread puo' inserirsi in mezzo.
 * - k_thread_create con struct k_thread + stack: creare thread a runtime (Modulo 3).
 * - Priorita': numero PIU' BASSO = piu' importante. 5 batte 7 batte 10.
 * - Inversione di priorita': ALTA aspetta MEDIA, che non c'entra nulla.
 * - Ereditarieta' di priorita': il k_mutex alza temporaneamente BASSA alla priorita' di
 *   ALTA mentre ALTA aspetta il mutex. Il semaforo non puo' farlo: non ha proprietario.
 *
 * COME VERIFICARE DA SOLO
 *     .\verifica.ps1 modulo4\08_atomic_inversione
 *
 * ERRORI TIPICI
 * - Usare k_msleep al posto di lavora_ms in bassa/media: dormendo lasciano la CPU e
 *   l'inversione non si vede (non c'e' nessuno che "ruba" la CPU).
 * - Riusare uno stack mentre il thread precedente e' ancora vivo: fai sempre join prima.
 * - Stampare attesa_alta (int64_t) con %d: usa %lld.
 *
 * BONUS (facoltativo)
 * - Aggiungi CONFIG_THREAD_NAME=y e in bassa() stampa k_thread_priority_get(k_current_get())
 *   subito dopo lavora_ms(300), prima di liberare: con il mutex vedrai 5, non 10!
 * - Calcola a mano i tempi attesi dei due scenari: 750 e 250 sono giusti? Da dove viene
 *   la differenza di 500?
 *
 * Scrivi il tuo codice qui sotto.
 */

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys/atomic.h>

#define N_INCREMENTI 10000
#define DIM_STACK    1024

#define PRIO_ALTA  5
#define PRIO_MEDIA 7
#define PRIO_BASSA 10

/* --- strutture per i thread creati a runtime ---------------------------------------- */
K_THREAD_STACK_DEFINE(stack_alta, DIM_STACK);
K_THREAD_STACK_DEFINE(stack_media, DIM_STACK);
K_THREAD_STACK_DEFINE(stack_bassa, DIM_STACK);
static struct k_thread th_alta, th_media, th_bassa;

/* --- Parte A -------------------------------------------------------------------------- */
static atomic_t contatore = ATOMIC_INIT(0);

static void incrementa_atomico(void *p1, void *p2, void *p3)
{
	ARG_UNUSED(p1);
	ARG_UNUSED(p2);
	ARG_UNUSED(p3);

	/* TODO: N_INCREMENTI volte atomic_inc + k_yield */
}

/* --- Parte B -------------------------------------------------------------------------- */
K_SEM_DEFINE(sem_risorsa, 1, 1);
K_MUTEX_DEFINE(mutex_risorsa);

static bool usa_mutex;
static int64_t attesa_alta;

/* Tiene la CPU occupata per circa ms millisecondi (lavoro "vero", non sleep).
 * A piccoli pezzi da 1 ms: se il thread viene interrotto, il lavoro restante resta da fare.
 */
static void lavora_ms(int ms)
{
	for (int i = 0; i < ms; i++) {
		k_busy_wait(1000);
	}
}

static void prendi_risorsa(void)
{
	/* TODO: mutex o semaforo a seconda di usa_mutex */
}

static void libera_risorsa(void)
{
	/* TODO: mutex o semaforo a seconda di usa_mutex */
}

static void alta(void *p1, void *p2, void *p3)
{
	ARG_UNUSED(p1);
	ARG_UNUSED(p2);
	ARG_UNUSED(p3);

	/* TODO */
}

static void media(void *p1, void *p2, void *p3)
{
	ARG_UNUSED(p1);
	ARG_UNUSED(p2);
	ARG_UNUSED(p3);

	/* TODO */
}

static void bassa(void *p1, void *p2, void *p3)
{
	ARG_UNUSED(p1);
	ARG_UNUSED(p2);
	ARG_UNUSED(p3);

	/* TODO */
}

static int64_t esegui_scenario(bool mutex)
{
	usa_mutex = mutex;
	attesa_alta = 0;

	/* TODO: k_thread_create dei tre thread, join, stampa, return attesa_alta */

	return attesa_alta;
}

int main(void)
{
	printk("Parte A\n");
	/* TODO: due thread incrementa_atomico, join, stampa il totale */

	printk("Parte B\n");
	/* TODO: esegui_scenario(false), esegui_scenario(true), confronto */

	/* Tolgono gli avvisi "funzione non usata" finche' lo scheletro e' vuoto:
	 * cancellale quando hai scritto il tuo codice.
	 */
	ARG_UNUSED(incrementa_atomico);
	ARG_UNUSED(alta);
	ARG_UNUSED(media);
	ARG_UNUSED(bassa);
	ARG_UNUSED(esegui_scenario);
	ARG_UNUSED(lavora_ms);
	ARG_UNUSED(prendi_risorsa);
	ARG_UNUSED(libera_risorsa);
	ARG_UNUSED(th_alta);
	ARG_UNUSED(th_media);
	ARG_UNUSED(th_bassa);
	ARG_UNUSED(contatore);

	return 0;
}
