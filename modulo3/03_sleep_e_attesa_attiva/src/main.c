/*
 * Esercizio 3 (Modulo 3) - Dormire o aspettare attivamente?
 *
 * Teoria: teoria/04_thread_scheduling.md, sezione 6.
 *
 * OBIETTIVO
 * Misurare la differenza tra k_msleep (il thread va in attesa, la CPU e' libera per
 * altri) e k_busy_wait (la CPU gira a vuoto). Usare gli orologi del kernel:
 * k_uptime_get e il contatore hardware k_cycle_get_32.
 *
 * PASSI
 * 1. Thread "sfondo" a priorita' 14 (la meno importante tra le preemptive) GIA'
 *    SCRITTO qui sotto: incrementa senza sosta la variabile giri_sfondo. Gira solo
 *    quando nessun altro thread ha qualcosa da fare.
 * 2. In main() stampa la frequenza del contatore hardware:
 *        printk("Cicli al secondo: %u\n", sys_clock_hw_cycles_per_sec());
 * 3. Misura k_msleep(100):
 *      - salva  int64_t t0 = k_uptime_get();  uint32_t c0 = k_cycle_get_32();
 *        uint32_t g0 = giri_sfondo;
 *      - k_msleep(100);
 *      - calcola durata in ms (k_uptime_get() - t0), cicli trascorsi
 *        (k_cycle_get_32() - c0) e giri dello sfondo (giri_sfondo - g0);
 *      - stampa  "sleep: %lld ms, %u cicli, sfondo avanzato di %u giri"
 *        (%lld per int64_t, %u per uint32_t).
 * 4. Fai lo stesso con k_busy_wait(100000) (microsecondi!) e stampa
 *        "busy_wait: %lld ms, %u cicli, sfondo avanzato di %u giri"
 * 5. Ripeti le due misure 3 volte in un ciclo for, poi stampa "Fine misure".
 * 6. Confronta: durate simili, ma durante busy_wait lo sfondo non avanza MAI (0 giri):
 *    main (priorita' 0) non ha mai lasciato la CPU.
 * 7. Lancia la verifica.
 *
 * OUTPUT ATTESO (i numeri possono variare, tranne lo 0)
 *     Cicli al secondo: 168000000
 *     sleep: 100 ms, 16800000 cicli, sfondo avanzato di 412345 giri
 *     busy_wait: 100 ms, 16800000 cicli, sfondo avanzato di 0 giri
 *     ... (3 volte)
 *     Fine misure
 *
 * CONCETTI NUOVI
 * - k_msleep: il thread passa allo stato "in attesa"; lo scheduler esegue altri.
 * - k_busy_wait: ciclo che consuma CPU; solo per attese di microsecondi richieste da
 *   un componente hardware.
 * - k_uptime_get(): ms dall'avvio (int64_t). k_cycle_get_32(): contatore hardware a
 *   32 bit (qui i cicli di clock della CPU, contati con il SysTick a 168 MHz:
 *   100 ms = 16.800.000 cicli).
 * - La differenza tra due letture di un contatore unsigned funziona anche se nel
 *   frattempo il contatore e' "ripartito da 0" (aritmetica modulo 2^32). A 168 MHz
 *   un contatore a 32 bit riparte da 0 ogni 2^32 / 168e6 = circa 25,6 secondi.
 * - Tempo virtuale: in Renode le durate sono quelle "della scheda"; il numero di giri
 *   dello sfondo dipende dalla velocita' del simulatore.
 *
 * COME VERIFICARE DA SOLO
 *     .\verifica.ps1 modulo3\03_sleep_e_attesa_attiva
 *
 * ERRORI TIPICI
 * - k_busy_wait(100): sono 100 MICROsecondi, non millisecondi.
 * - Stampare un int64_t con %d: numero sbagliato. Usa %lld (e il valore e' long long).
 * - Leggere giri_sfondo senza volatile: il compilatore potrebbe tenere una copia
 *   vecchia in un registro (la variabile e' gia' dichiarata volatile).
 *
 * BONUS (facoltativo)
 * - Converti i cicli in microsecondi con k_cyc_to_us_floor32(cicli).
 * - Cambia la priorita' di sfondo a 0 (uguale a main): durante busy_wait avanza ora?
 *   (Suggerimento: dipende dal time slicing, esercizio 6.)
 * - Misura quanto dura una printk di 40 caratteri con k_cycle_get_32.
 *
 * Scrivi il tuo codice qui sotto.
 */

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

static volatile uint32_t giri_sfondo;

/* Thread di sfondo: gia' pronto, non modificarlo. */
static void sfondo(void *p1, void *p2, void *p3)
{
	ARG_UNUSED(p1);
	ARG_UNUSED(p2);
	ARG_UNUSED(p3);

	while (1) {
		giri_sfondo++;
	}
}

K_THREAD_DEFINE(id_sfondo, 512, sfondo, NULL, NULL, NULL, 14, 0, 0);

int main(void)
{
	/* TODO: passi 2, 3, 4 e 5 */

	return 0;
}
