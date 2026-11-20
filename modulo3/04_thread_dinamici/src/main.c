/*
 * Esercizio 4 (Modulo 3) - Thread creati a runtime
 *
 * Teoria: teoria/04_thread_scheduling.md, sezione 7.
 *
 * OBIETTIVO
 * Creare durante l'esecuzione 3 thread "lavoratori" che eseguono la stessa funzione
 * con parametri diversi, aspettare che finiscano tutti con k_thread_join e solo dopo
 * stampare il riepilogo.
 *
 * PASSI
 * 1. Stack e descrittori sono gia' dichiarati qui sotto (N_LAVORATORI = 3):
 *        K_THREAD_STACK_ARRAY_DEFINE(stacks, N_LAVORATORI, STACK_SIZE);
 *        static struct k_thread lavoratori[N_LAVORATORI];
 *    Anche se il thread nasce a runtime, la sua memoria e' riservata in compilazione:
 *    in embedded niente sorprese sulla RAM.
 * 2. Scrivi la funzione  void lavoro(void *p1, void *p2, void *p3)  che:
 *      - ricava id = (int)(intptr_t)p1 e n = (int)(intptr_t)p2;
 *      - calcola la somma 1 + 2 + ... + n con un ciclo for (usa un uint32_t);
 *      - stampa "lavoratore %d: somma 1..%d = %u".
 *    E poi ritorna: il thread termina.
 * 3. In main():
 *      - stampa "main: creo i lavoratori";
 *      - con un ciclo for i = 0..2 crea i thread con k_thread_create:
 *            k_thread_create(&lavoratori[i], stacks[i],
 *                            K_THREAD_STACK_SIZEOF(stacks[i]),
 *                            lavoro, (void *)(intptr_t)(i + 1),
 *                            (void *)(intptr_t)limiti[i], NULL,
 *                            PRIORITA, 0, K_NO_WAIT);
 *      - con un altro ciclo for aspetta ognuno: k_thread_join(&lavoratori[i], K_FOREVER);
 *      - stampa "main: tutti i lavoratori hanno finito".
 * 4. Esegui. In che ordine stampano i lavoratori? Perche' main riesce a crearli tutti
 *    prima che inizino? (main ha priorita' 0, i lavoratori PRIORITA = 5.)
 * 5. Lancia la verifica.
 *
 * OUTPUT ATTESO
 *     main: creo i lavoratori
 *     lavoratore 1: somma 1..10 = 55
 *     lavoratore 2: somma 1..100 = 5050
 *     lavoratore 3: somma 1..1000 = 500500
 *     main: tutti i lavoratori hanno finito
 *
 * CONCETTI NUOVI
 * - k_thread_create: crea un thread a runtime con stack e struttura gia' riservati.
 * - Parametri void *: un "puntatore a qualunque cosa". Per passare un intero si
 *   converte: (void *)(intptr_t)n, e indietro (int)(intptr_t)p. intptr_t e' un intero
 *   grande quanto un puntatore.
 * - k_thread_join: aspetta la fine di un thread (come join in Python threading).
 * - Un thread termina quando la sua funzione ritorna; poi struttura e stack si possono
 *   riusare.
 * - K_NO_WAIT / K_FOREVER: i "timeout" del kernel: non aspettare / aspetta per sempre.
 *
 * COME VERIFICARE DA SOLO
 *     .\verifica.ps1 modulo3\04_thread_dinamici
 *
 * ERRORI TIPICI
 * - Passare &i (indirizzo della variabile del ciclo) come parametro: quando il thread
 *   parte, i e' gia' cambiata. Passa il VALORE convertito.
 * - Usare lo stesso stack per due thread vivi contemporaneamente: si sovrascrivono a
 *   vicenda, crash casuali.
 * - Stampare "tutti hanno finito" senza join: compare prima dei risultati.
 * - Somma in int16_t: 500500 non ci sta (overflow, Modulo 1).
 *
 * BONUS (facoltativo)
 * - Passa invece un puntatore a una struct { int id; int n; uint32_t risultato; }
 *   (array globale di struct) e fai stampare i risultati a main dopo i join.
 * - Dai priorita' diverse ai lavoratori (es. 7, 6, 5): cambia l'ordine delle stampe?
 * - Crea i thread con K_MSEC(100 * i) come ultimo argomento: partenza ritardata.
 *
 * Scrivi il tuo codice qui sotto.
 */

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

#define N_LAVORATORI 3
#define STACK_SIZE   1024
#define PRIORITA     5

K_THREAD_STACK_ARRAY_DEFINE(stacks, N_LAVORATORI, STACK_SIZE);
static struct k_thread lavoratori[N_LAVORATORI];

static const int limiti[N_LAVORATORI] = {10, 100, 1000};

/* TODO: passo 2 - la funzione lavoro */

int main(void)
{
	/* TODO: passo 3 */

	return 0;
}
