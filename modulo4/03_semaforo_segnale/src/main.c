/*
 * Esercizio 3 (Modulo 4) - Semaforo come segnale
 *
 * Teoria: teoria/05_sincronizzazione.md, sezione 4.
 *
 * OBIETTIVO
 * Un thread "produttore" prepara un dato ogni secondo e avvisa un thread "consumatore"
 * con un semaforo. Il consumatore dorme finche' non arriva il segnale (non consuma CPU
 * controllando in continuazione). Dopo 5 dati il produttore smette: il consumatore se
 * ne accorge grazie al timeout.
 *
 * PASSI
 * 1. Il semaforo e' gia' definito: K_SEM_DEFINE(dato_pronto, 0, 1);
 *    (valore iniziale 0 = "niente da consumare", massimo 1).
 * 2. produttore(): per 5 volte
 *        k_msleep(1000);
 *        dato = dato + 10;   (simula una nuova lettura: 10, 20, 30...)
 *        stampa "Produttore: dato %d pronto"
 *        k_sem_give(&dato_pronto);
 *    poi stampa "Produttore: ho finito" ed esce dalla funzione.
 * 3. consumatore(): ciclo infinito
 *        int ret = k_sem_take(&dato_pronto, K_MSEC(1500));
 *    - se ret == 0: incrementa ricevuti e stampa "Ricevuto segnale N: dato D"
 *    - se ret == -EAGAIN: stampa "Timeout: nessun segnale da 1500 ms" ed esci dal ciclo
 *      (return dalla funzione).
 * 4. Esegui: i segnali arrivano ogni secondo, poi dopo l'ultimo il timeout.
 *
 * OUTPUT ATTESO
 *     Produttore: dato 10 pronto
 *     Ricevuto segnale 1: dato 10
 *     ...
 *     Produttore: dato 50 pronto
 *     Ricevuto segnale 5: dato 50
 *     Produttore: ho finito
 *     Timeout: nessun segnale da 1500 ms
 *
 * CONCETTI NUOVI
 * - K_SEM_DEFINE(nome, iniziale, massimo).
 * - k_sem_give: "c'e' qualcosa" (+1). k_sem_take: aspetta che ci sia qualcosa (-1).
 * - Il consumatore bloccato in k_sem_take NON usa la CPU: lo scheduler non lo esegue
 *   finche' il semaforo non viene dato.
 * - Valori di ritorno: 0 = preso, -EAGAIN = timeout scaduto, -EBUSY = K_NO_WAIT e niente
 *   disponibile. Le costanti di errore stanno in <errno.h> (incluso da kernel.h).
 *
 * COME VERIFICARE DA SOLO
 *     .\verifica.ps1 modulo4\03_semaforo_segnale
 *
 * ERRORI TIPICI
 * - Semaforo con valore iniziale 1: il consumatore parte subito con un dato che non c'e'.
 * - Confrontare ret con EAGAIN (positivo) invece di -EAGAIN.
 * - Variabile "dato" letta dal consumatore e scritta dal produttore: qui va bene perche'
 *   il produttore la scrive PRIMA del give e il consumatore la legge DOPO il take.
 *   Se il produttore fosse piu' veloce del consumatore? (vedi BONUS)
 *
 * BONUS (facoltativo)
 * - Fai produrre al produttore ogni 100 ms mentre il consumatore impiega 300 ms per dato
 *   (k_msleep dopo la stampa): quanti dati si perdono? Perche' il semaforo con massimo 1
 *   non "conta" i dati in arrivo? Cosa succede con massimo 10?
 *   (Nell'esercizio 5 vedrai lo strumento giusto: la coda di messaggi.)
 *
 * Scrivi il tuo codice qui sotto.
 */

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

#define DIM_STACK 1024

K_SEM_DEFINE(dato_pronto, 0, 1);

/* Dato "prodotto": scritto dal produttore prima del give, letto dal consumatore dopo il take. */
static int dato;

static void produttore(void *p1, void *p2, void *p3)
{
	ARG_UNUSED(p1);
	ARG_UNUSED(p2);
	ARG_UNUSED(p3);

	/* TODO: 5 volte: dormi 1 s, aggiorna dato, stampa, k_sem_give */
}

static void consumatore(void *p1, void *p2, void *p3)
{
	ARG_UNUSED(p1);
	ARG_UNUSED(p2);
	ARG_UNUSED(p3);

	/* TODO: ciclo con k_sem_take(&dato_pronto, K_MSEC(1500)) e gestione del timeout */
}

K_THREAD_DEFINE(t_produttore, DIM_STACK, produttore, NULL, NULL, NULL, 6, 0, 0);
K_THREAD_DEFINE(t_consumatore, DIM_STACK, consumatore, NULL, NULL, NULL, 5, 0, 0);

int main(void)
{
	printk("Semaforo come segnale: avvio\n");
	return 0;
}
