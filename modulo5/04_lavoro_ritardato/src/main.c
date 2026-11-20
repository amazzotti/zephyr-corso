/*
 * Esercizio 4 (Modulo 5) - Lavoro ritardato e timeout
 *
 * Teoria: teoria/06_tempo_lavoro_differito.md, sezione 7.
 *
 * OBIETTIVO
 * Un sensore invia un dato al secondo. Se per 3 secondi non arriva nulla, deve scattare
 * un allarme. Si realizza con un k_work_delayable "rinviato" a ogni dato: finche' i dati
 * arrivano l'allarme non parte mai; quando smettono, parte da solo.
 *
 * PASSI
 * 1. Gia' pronto: K_WORK_DELAYABLE_DEFINE(timeout_dati, allarme);
 * 2. allarme(): allarmi++; stampa "ALLARME: nessun dato da 3 s (allarme %u)".
 * 3. sensore() (thread):
 *    a. per i da 1 a 5: k_msleep(1000); stampa "Dato %d ricevuto";
 *       k_work_reschedule(&timeout_dati, K_SECONDS(TIMEOUT_S));
 *    b. stampa "Sensore: guasto simulato"; k_msleep(5000);
 *    c. stampa "Sensore: ripristinato"; per i da 6 a 8 come al punto a.
 *    d. stampa "Sensore: spento" ed esci.
 * 4. In main(): fai partire la sorveglianza subito, prima ancora del primo dato:
 *        k_work_schedule(&timeout_dati, K_SECONDS(TIMEOUT_S));
 * 5. Disegna la linea del tempo (dato, scadenza spostata, allarme) e verifica che gli
 *    allarmi cadano dove te li aspetti (circa a 8 s e a 16 s).
 *
 * OUTPUT ATTESO
 *     Sorveglianza attiva: timeout 3 s
 *     Dato 1 ricevuto
 *     ...
 *     Dato 5 ricevuto
 *     Sensore: guasto simulato
 *     ALLARME: nessun dato da 3 s (allarme 1)
 *     Sensore: ripristinato
 *     Dato 6 ricevuto
 *     Dato 7 ricevuto
 *     Dato 8 ricevuto
 *     Sensore: spento
 *     ALLARME: nessun dato da 3 s (allarme 2)
 *
 * CONCETTI NUOVI
 * - k_work_delayable: un lavoro che la work queue esegue dopo un ritardo.
 * - k_work_schedule: programma SOLO se non e' gia' programmato (non sposta la scadenza).
 * - k_work_reschedule: sposta SEMPRE la scadenza a "adesso + ritardo".
 * - Schema "watchdog software": ogni segno di vita sposta in avanti la scadenza.
 * - Dopo che l'allarme e' scattato, il lavoro non e' piu' programmato: il prossimo
 *   reschedule lo riprogramma (riarma la sorveglianza).
 *
 * COME VERIFICARE DA SOLO
 *     .\verifica.ps1 modulo5\04_lavoro_ritardato
 *
 * ERRORI TIPICI
 * - Usare k_work_schedule al posto di reschedule nel sensore: la scadenza non si sposta
 *   e l'allarme scatta 3 s dopo l'avvio anche se i dati arrivano (vedi DOMANDE).
 * - Dimenticare il passo 4: se il sensore non manda MAI niente, nessun allarme.
 *
 * DOMANDE (rispondi in fondo al file)
 * a. Sostituisci reschedule con schedule nel sensore: quando scattano gli allarmi?
 *    Perche'?
 * b. Perche' dopo "ripristinato" non serve fare di nuovo il passo 4?
 *
 * BONUS (facoltativo)
 * - Quando il sensore si ripristina dopo un allarme, stampa "Dati di nuovo regolari"
 *   (ti serve una variabile "in_allarme").
 * - Usa k_work_delayable_remaining_get(&timeout_dati) per stampare a ogni dato quanto
 *   mancava all'allarme.
 *
 * Scrivi il tuo codice qui sotto.
 */

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

#define TIMEOUT_S 3
#define DIM_STACK 1024

static uint32_t allarmi;

static void allarme(struct k_work *w)
{
	ARG_UNUSED(w);
	/* TODO */
}

K_WORK_DELAYABLE_DEFINE(timeout_dati, allarme);

static void sensore(void *p1, void *p2, void *p3)
{
	ARG_UNUSED(p1);
	ARG_UNUSED(p2);
	ARG_UNUSED(p3);

	/* TODO: 5 dati, guasto, ripristino, 3 dati, spento */
}

K_THREAD_DEFINE(t_sensore, DIM_STACK, sensore, NULL, NULL, NULL, 5, 0, 0);

int main(void)
{
	printk("Sorveglianza attiva: timeout %d s\n", TIMEOUT_S);

	/* TODO: k_work_schedule della sorveglianza */
	ARG_UNUSED(allarmi);   /* TODO: togli questa riga quando usi allarmi */

	return 0;
}

/*
 * RISPOSTE ALLE DOMANDE
 * a.
 * b.
 */
