/*
 * Esercizio 2 (Modulo 4) - Mutex
 *
 * Teoria: teoria/05_sincronizzazione.md, sezioni 3 e 5.
 *
 * OBIETTIVO
 * Correggere la race condition dell'esercizio 1 proteggendo la sezione critica con un
 * mutex. Il totale deve tornare esattamente 20000, anche con il k_yield() "cattivo"
 * dentro la sezione critica.
 *
 * PASSI
 * 1. Copia in incrementa() il ciclo che hai scritto nell'esercizio 1
 *    (copia = contatore; k_yield(); contatore = copia + 1;).
 * 2. Il mutex e' gia' definito sotto:  K_MUTEX_DEFINE(lucchetto);
 *    Prima della lettura prendilo:      k_mutex_lock(&lucchetto, K_FOREVER);
 *    Dopo la scrittura liberalo:        k_mutex_unlock(&lucchetto);
 * 3. Conta in una variabile globale quante volte un thread ha trovato il mutex gia'
 *    occupato: prova prima con
 *        if (k_mutex_lock(&lucchetto, K_NO_WAIT) != 0) {
 *            attese++;                          (era occupato)
 *            k_mutex_lock(&lucchetto, K_FOREVER);
 *        }
 *    (attese la modifica solo chi ha appena fallito il lock: e' protetta? pensaci,
 *    vedi ERRORI TIPICI.)
 * 4. In main(): join dei due thread, poi stampa totale e attese.
 *
 * OUTPUT ATTESO
 *     Avvio: due thread con mutex, 10000 incrementi ciascuno
 *     Totale: 20000 (atteso 20000)
 *     Lock trovato occupato: 19999 volte      (circa: dipende dall'alternanza)
 *
 * CONCETTI NUOVI
 * - K_MUTEX_DEFINE: crea un mutex statico gia' inizializzato.
 * - k_mutex_lock(&m, timeout) / k_mutex_unlock(&m): entrare e uscire dalla sezione
 *   critica. Il valore di ritorno 0 = preso; diverso da 0 = timeout scaduto.
 * - K_NO_WAIT / K_FOREVER: non aspettare / aspettare per sempre.
 * - Quando A tiene il mutex e fa k_yield, B prova a prenderlo, si BLOCCA e la CPU
 *   torna ad A: per questo il risultato ora e' giusto.
 *
 * COME VERIFICARE DA SOLO
 *     .\verifica.ps1 modulo4\02_mutex
 * Il totale deve essere esattamente 20000 e non deve comparire "persi".
 *
 * ERRORI TIPICI
 * - Dimenticare l'unlock (o fare return prima dell'unlock): l'altro thread resta
 *   bloccato per sempre e il programma non stampa piu' nulla (deadlock).
 * - Mettere lock/unlock fuori dal ciclo for: funziona, ma un thread fa tutti i suoi
 *   10000 giri mentre l'altro aspetta. Sezione critica = il minimo indispensabile.
 * - attese++ fuori dal mutex e' a sua volta una race condition! Qui pero' viene
 *   eseguito subito prima di un k_mutex_lock bloccante senza yield in mezzo: chi lo
 *   interrompe? Rispondi nelle DOMANDE.
 *
 * DOMANDE (rispondi in fondo al file)
 * a. Quanto e' piu' lento con il mutex? Misura con k_uptime_get() prima e dopo i join,
 *    nei due esercizi.
 * b. Perche' "attese" vale quasi 20000, cioe' quasi ogni lock trova il mutex occupato?
 *    Suggerimento: quando A fa unlock e B sta aspettando, il kernel passa SUBITO il
 *    mutex a B; A pero' continua a girare (stessa priorita') e al giro dopo...
 *
 * BONUS (facoltativo)
 * - Togli il k_yield: quante attese conti adesso? Perche'?
 * - Prova k_mutex_lock(&lucchetto, K_MSEC(1)) e stampa quando restituisce -EAGAIN.
 *
 * Scrivi il tuo codice qui sotto.
 */

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

#define N_INCREMENTI 10000
#define DIM_STACK    1024
#define PRIORITA     5

static int contatore;
static int attese;

K_MUTEX_DEFINE(lucchetto);

static void incrementa(void *p1, void *p2, void *p3)
{
	ARG_UNUSED(p1);
	ARG_UNUSED(p2);
	ARG_UNUSED(p3);

	/* TODO: lo stesso ciclo dell'esercizio 1, con la sezione critica protetta dal mutex */
}

K_THREAD_DEFINE(thread_a, DIM_STACK, incrementa, NULL, NULL, NULL, PRIORITA, 0, 0);
K_THREAD_DEFINE(thread_b, DIM_STACK, incrementa, NULL, NULL, NULL, PRIORITA, 0, 0);

int main(void)
{
	printk("Avvio: due thread con mutex, %d incrementi ciascuno\n", N_INCREMENTI);

	/* TODO: join dei due thread, poi stampa "Totale: ..." e "Lock trovato occupato: ..." */

	return 0;
}

/*
 * RISPOSTE ALLE DOMANDE
 * a.
 * b.
 */
