/*
 * Esercizio 3 (Modulo 6) - Debounce
 *
 * Teoria: teoria/07_periferiche_driver.md, sezioni 5 e 11 (macro di Renode);
 *         teoria/06_tempo_lavoro_differito.md (k_timer, work queue).
 *
 * OBIETTIVO
 * Contare SOLO le pressioni vere di sw0, ignorando i rimbalzi meccanici: una pressione
 * con rimbalzi (pin che oscilla per qualche ms) deve contare 1. Tecnica: si campiona
 * ogni 10 ms e si accetta un nuovo stato solo quando e' rimasto UGUALE per 3 campioni
 * consecutivi (30 ms). E' il debounce "per campionamento", molto usato nei firmware.
 *
 * (Come nell'esercizio 2, il pulsante si campiona con un timer perche' in Renode gli
 * interrupt GPIO non scattano: vedi teoria, sezione 4.)
 *
 * PASSI
 * 1. Parti dall'esercizio 2 (pulsante, LED, k_timer ogni 10 ms).
 * 2. Costante e variabili static nella funzione del timer (o globali static):
 *        #define CAMPIONI_STABILI 3
 *        static int stabile = 0;      (ultimo stato ACCETTATO: 0 rilasciato, 1 premuto)
 *        static int candidato = 0;    (ultima lettura grezza)
 *        static int uguali = 0;       (quante letture consecutive uguali a candidato)
 * 3. Nella funzione del timer, a ogni campione:
 *      - int letto = gpio_pin_get_dt(&btn);
 *      - se letto == candidato: uguali++;  altrimenti: candidato = letto; uguali = 1;
 *      - se uguali >= CAMPIONI_STABILI e candidato != stabile:
 *            stabile = candidato;
 *            se stabile == 1 -> e' una PRESSIONE valida, se 0 -> un RILASCIO valido:
 *            segnalalo al thread (es. k_msgq_put di un int, o due semafori, o un
 *            k_work_submit). Ricorda: siamo in un interrupt, niente printk.
 * 4. Nel thread principale (o in una funzione della work queue): per ogni evento
 *      - pressione: incrementa il contatore, inverti led0, stampa "Pressioni valide: N";
 *      - rilascio: stampa "Rilasciato".
 * 5. Stampa "Pronto: premi sw0" dopo la configurazione, prima del ciclo.
 * 6. ESPERIMENTO in Renode (.\esegui.ps1): definisci nel monitor la macro "rimbalzo"
 *    della teoria (sezione 11) e lanciala con runMacro $rimbalzo, poi
 *        sysbus.gpio0.sw0 Release
 *    Quante pressioni conta questo programma? E quello dell'esercizio 2 con la stessa
 *    macro? Scrivi i risultati in fondo a questo commento. (I comandi della macro
 *    arrivano a distanza di pochissimo tempo virtuale: il campionamento a 10 ms spesso
 *    non li vede neppure. Prova anche a mettere CAMPIONI_STABILI a 1 e a ridurre il
 *    periodo del timer a 1 ms: cosa cambia?)
 *
 * OUTPUT ATTESO (due pressioni pulite, oppure due pressioni con rimbalzi)
 *     Pronto: premi sw0
 *     Pressioni valide: 1
 *     Rilasciato
 *     Pressioni valide: 2
 *     Rilasciato
 *
 * CONCETTI NUOVI
 * - Rimbalzo dei contatti: per qualche ms dopo la pressione il pin oscilla 0/1/0/1.
 *   Chi conta ogni cambiamento conta troppo.
 * - Debounce per campionamento: uno stato e' "vero" solo se resta stabile per N
 *   campioni. Ritardo introdotto: N x periodo (qui 30 ms), impercettibile per una persona.
 * - Macchina a stati con memoria (variabili static): stabile / candidato / contatore.
 * - Alternativa con interrupt (scheda vera): interrupt su entrambi i fronti +
 *   k_work_reschedule di un k_work_delayable di 30 ms (teoria, sezione 5).
 *
 * COME VERIFICARE DA SOLO
 *     .\verifica.ps1 modulo6\03_debounce
 * La verifica fa due pressioni pulite (in Renode la durata dei rimbalzi in tempo
 * virtuale non si controlla con precisione: i rimbalzi li provi a mano, passo 6).
 *
 * ERRORI TIPICI
 * - Azzerare "uguali" a 0 invece che a 1 quando cambia il candidato: serve un campione
 *   in piu' del previsto.
 * - Segnalare l'evento a ogni campione stabile invece che solo quando cambia "stabile":
 *   una pressione lunga genera centinaia di eventi.
 * - Contare anche il rilascio come pressione (manca il controllo stabile == 1).
 * - CAMPIONI_STABILI troppo grande (es. 50 = 500 ms): due pressioni rapide vere
 *   diventano una.
 *
 * BONUS (facoltativo)
 * - Riconosci la pressione LUNGA: se lo stato stabile resta 1 per piu' di 1 s stampa
 *   "Pressione lunga" (conta i campioni nella funzione del timer, oppure usa un
 *   k_work_delayable programmato alla pressione e cancellato al rilascio).
 * - Rendi il debounce una piccola "libreria" (debounce.c/.h) riusabile per piu' pulsanti,
 *   con una struct per pulsante (stabile, candidato, uguali): ti servira' nel Modulo 8.
 * - Variante "integratore": un contatore che sale (max 3) quando legge 1 e scende
 *   (min 0) quando legge 0; lo stato diventa 1 a 3 e 0 a 0. Confrontala con la tua.
 *
 * RISULTATI DELL'ESPERIMENTO (passo 6):
 *   esercizio 2 con la macro rimbalzo: ...
 *   esercizio 3 con la macro rimbalzo: ...
 *
 * Scrivi il tuo codice qui sotto.
 */

#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/printk.h>

/* TODO: gpio_dt_spec, stato del debounce, funzione del timer, coda/segnali (passi 1-3) */

int main(void)
{
	/* TODO: configurazione di pulsante e LED, avvio del timer, ciclo (passi 1, 4, 5) */

	return 0;
}
