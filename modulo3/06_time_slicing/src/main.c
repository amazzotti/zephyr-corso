/*
 * Esercizio 6 (Modulo 3) - Time slicing
 *
 * Teoria: teoria/04_thread_scheduling.md, sezione 8.
 *
 * OBIETTIVO
 * Due thread della STESSA priorita' che lavorano senza mai dormire: senza time slicing
 * il primo tiene la CPU finche' non ha finito; con il time slicing si alternano a
 * fette di 20 ms.
 *
 * PASSI
 * 1. Scrivi una sola funzione  void calcola(void *p1, void *p2, void *p3)  usata da
 *    entrambi i thread. p1 e' il nome ("A" o "B"): const char *nome = p1;
 *    Il thread fa 10 "blocchi" di lavoro: per ogni blocco
 *        k_busy_wait(50000);                      (50 ms di calcolo simulato)
 *        printk("%s: blocco %d\n", nome, i);
 *    e alla fine stampa "%s: fine".
 * 2. Crea due thread con K_THREAD_DEFINE, ENTRAMBI con priorita' 5, che usano calcola
 *    con p1 = "A" e p1 = "B". Esegui con CONFIG_TIMESLICE_SIZE=0: prima tutti i blocchi
 *    di uno dei due e la sua "fine", poi tutti quelli dell'altro.
 * 3. In prj.conf metti CONFIG_TIMESLICE_SIZE=20 e riesegui (con -Pulito): ora i blocchi
 *    di A e B si alternano. A ogni scadenza dei 20 ms il kernel passa la CPU all'altro
 *    thread di pari priorita'.
 * 4. Domanda: con il time slicing, un thread di priorita' 4 che si sveglia deve
 *    aspettare la fine della fetta? (No: le priorita' vincono sempre. Teoria sez. 8.)
 * 5. Lancia la verifica con CONFIG_TIMESLICE_SIZE=20.
 *
 * OUTPUT ATTESO (con time slicing; l'alternanza esatta puo' variare)
 *     A: blocco 0
 *     B: blocco 0
 *     A: blocco 1
 *     B: blocco 1
 *     ...
 *     A: fine
 *     B: fine
 *
 * CONCETTI NUOVI
 * - Time slicing: divisione del tempo tra thread di PARI priorita' sempre pronti.
 * - Una stessa funzione puo' essere eseguita da piu' thread: ogni thread ha il suo
 *   stack, quindi le sue variabili locali (i, nome) sono separate.
 * - Preemption a meta' di un'operazione: un thread puo' essere interrotto in qualunque
 *   punto, anche a meta' di una printk. Se vedi righe "mescolate" (A: bloB: blocco),
 *   e' proprio questo. Nel Modulo 4 vedremo come proteggere le sezioni critiche.
 *
 * COME VERIFICARE DA SOLO
 *     .\verifica.ps1 modulo3\06_time_slicing -Pulito
 * Controlla che entrambi i thread inizino a lavorare prima che uno dei due abbia
 * finito. (Chi parte per primo, A o B, non e' garantito.)
 *
 * ERRORI TIPICI
 * - Priorita' diverse ai due thread: il time slicing non c'entra piu', vince il piu'
 *   importante.
 * - Mettere un k_msleep nei blocchi: i thread si alternano comunque (perche' dormono)
 *   e non si vede l'effetto del time slicing.
 * - Cambiare prj.conf senza -Pulito e non vedere differenze.
 *
 * BONUS (facoltativo)
 * - Prova CONFIG_TIMESLICE_SIZE=200: quanti blocchi di fila fa ogni thread? Perche'?
 * - Metti CONFIG_TIMESLICE_PRIORITY=6 lasciando i thread a 5: il time slicing vale
 *   ancora per loro? (Leggi l'help dell'opzione in menuconfig.)
 *
 * Scrivi il tuo codice qui sotto.
 */

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

/* TODO: passi 1 e 2 */

int main(void)
{
	return 0;
}
