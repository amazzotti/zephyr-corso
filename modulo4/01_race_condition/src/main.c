/*
 * Esercizio 1 (Modulo 4) - Race condition
 *
 * Teoria: teoria/05_sincronizzazione.md, sezioni 1 e 2.
 *
 * OBIETTIVO
 * Due thread incrementano la stessa variabile globale 10000 volte ciascuno.
 * Il totale dovrebbe essere 20000, invece e' sbagliato. Devi provocare l'errore,
 * misurarlo e capire perche' succede.
 *
 * PASSI
 * 1. Nella funzione incrementa() scrivi un ciclo for di N_INCREMENTI giri.
 *    Dentro il ciclo NON scrivere contatore++, ma spezza l'operazione in tre passi,
 *    come fa la CPU:
 *        int copia = contatore;   (1. leggi)
 *        k_yield();               (lascia la CPU all'altro thread: simula un'interruzione
 *                                  che arriva nel momento peggiore)
 *        contatore = copia + 1;   (2-3. somma e scrivi)
 * 2. I due thread thread_a e thread_b sono gia' definiti sotto con K_THREAD_DEFINE,
 *    con la stessa priorita' ed eseguono entrambi incrementa().
 * 3. In main() aspetta che entrambi abbiano finito con
 *        k_thread_join(thread_a, K_FOREVER);
 *        k_thread_join(thread_b, K_FOREVER);
 *    (main ha priorita' piu' alta, quindi senza join stamperebbe subito 0).
 * 4. Stampa il risultato e, se e' sbagliato, quanti incrementi si sono persi:
 *        Totale: 10000 (atteso 20000)
 *        ATTENZIONE: persi 10000 incrementi
 * 5. Esegui e verifica. Poi rispondi nei commenti in fondo al file alle DOMANDE.
 *
 * OUTPUT ATTESO
 *     Avvio: due thread, 10000 incrementi ciascuno
 *     Totale: 10000 (atteso 20000)
 *     ATTENZIONE: persi 10000 incrementi
 * (il numero esatto dipende da come alterni i thread: l'importante e' che sia < 20000)
 *
 * CONCETTI NUOVI
 * - Variabile condivisa: una globale letta e scritta da piu' thread.
 * - Race condition: il risultato dipende da come lo scheduler alterna i thread.
 * - Sezione critica: il tratto "leggi-modifica-scrivi" che deve essere eseguito da un
 *   thread alla volta (lo proteggerai nell'esercizio 2).
 * - k_yield(): il thread rinuncia alla CPU a favore di thread pronti di pari priorita'.
 * - k_thread_join(): aspetta che un thread finisca (come thread.join() in Python).
 *
 * COME VERIFICARE DA SOLO
 *     .\verifica.ps1 modulo4\01_race_condition
 * Controlla che compaiano la riga del totale e la riga "persi N incrementi".
 *
 * ERRORI TIPICI
 * - Scrivere contatore++ senza k_yield: il totale torna giusto (vedi DOMANDE) e sembra
 *   che il problema non esista. Il bug c'e' lo stesso: si manifesta solo raramente.
 * - Dimenticare i k_thread_join: main stampa prima che i thread abbiano lavorato.
 * - Calcolare i persi come contatore - atteso: viene negativo.
 *
 * DOMANDE (rispondi qui in fondo, in un commento)
 * a. Disegna su carta la sequenza A legge / A yield / B legge / B yield / A scrive...
 *    con contatore che parte da 0: perche' si perde esattamente un incremento ogni due?
 * b. Togli k_yield() (lascia copia e scrittura): il totale e' giusto? Perche'?
 *    (suggerimento: thread di pari priorita', senza time slicing, chi interrompe chi?)
 * c. Su una scheda vera, senza k_yield, cosa potrebbe interrompere il thread in mezzo
 *    alla sequenza?
 *
 * BONUS (facoltativo)
 * - Al posto di k_yield() usa k_busy_wait(10) e aggiungi CONFIG_TIMESLICING=y con
 *   CONFIG_TIMESLICE_SIZE=1 nel prj.conf: il time slicing interrompe i thread a caso.
 *   Il numero di persi cambia? E' ancora sempre uguale tra un'esecuzione e l'altra?
 * - Aggiungi CONFIG_OUTPUT_DISASSEMBLY=y al prj.conf, ricompila, apri
 *   build/zephyr/zephyr.lst, cerca incrementa e trova le istruzioni ldr / adds / str.
 *
 * Scrivi il tuo codice qui sotto.
 */

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

#define N_INCREMENTI 10000
#define DIM_STACK    1024
#define PRIORITA     5

/* La variabile condivisa dai due thread. */
static int contatore;

static void incrementa(void *p1, void *p2, void *p3)
{
	ARG_UNUSED(p1);
	ARG_UNUSED(p2);
	ARG_UNUSED(p3);

	/* TODO: ciclo di N_INCREMENTI giri: leggi in copia, k_yield(), scrivi copia + 1 */
}

/* Due thread identici, stessa funzione, stessa priorita', partenza immediata. */
K_THREAD_DEFINE(thread_a, DIM_STACK, incrementa, NULL, NULL, NULL, PRIORITA, 0, 0);
K_THREAD_DEFINE(thread_b, DIM_STACK, incrementa, NULL, NULL, NULL, PRIORITA, 0, 0);

int main(void)
{
	printk("Avvio: due thread, %d incrementi ciascuno\n", N_INCREMENTI);

	/* TODO: aspetta la fine dei due thread con k_thread_join */

	/* TODO: stampa il totale e, se sbagliato, quanti incrementi sono stati persi */

	return 0;
}

/*
 * RISPOSTE ALLE DOMANDE
 * a.
 * b.
 * c.
 */
