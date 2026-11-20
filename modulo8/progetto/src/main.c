/*
 * Progetto finale (Modulo 8) - Nodo di monitoraggio della temperatura
 *
 * Teoria: teoria/09_progettazione.md
 *
 * Questa applicazione cresce un passo alla volta. Le consegne sono in:
 *     modulo8/01_specifica.md       specifica e architettura (prima di scrivere codice!)
 *     modulo8/02_acquisizione.md    thread del sensore e coda delle misure
 *     modulo8/03_elaborazione.md    media mobile, soglia con isteresi, LED di allarme
 *     modulo8/04_pulsante.md        conferma dell'allarme con il pulsante, LED di battito
 *     modulo8/05_shell_config.md    comandi della shell, soglia salvata con settings
 *     modulo8/06_robustezza.md      watchdog, guasti del sensore, dimensionamento
 *     modulo8/07_test_relazione.md  test della logica e relazione finale
 *
 * Organizzazione dei file (la crei tu, passo dopo passo; ogni .c in src/ viene
 * compilato automaticamente, vedi CMakeLists.txt):
 *     main.c        avvio: prepara l'hardware, avvia i moduli, stampa NODO AVVIATO
 *     misura.h      struct misura (condivisa tra i moduli)
 *     sensore.c/h   thread di acquisizione (parla con il BME280)
 *     filtro.c/h    media mobile (logica pura, testabile)
 *     allarme.c/h   macchina a stati dell'allarme con isteresi (logica pura, testabile)
 *     elabora.c/h   thread di elaborazione: coda -> filtro -> allarme -> uscite
 *     uscite.c/h    LED (allarme, battito)
 *     pulsante.c/h  pulsante con debounce
 *     comandi.c     comandi della shell
 *     config.c/h    soglie e settings
 *
 * Hardware (app.overlay): alias led_allarme (led0), led_battito (led1),
 * pulsante_conferma (sw0), sensore_temperatura (BME280 su i2c0, 0x76).
 *
 * Il comportamento richiesto e i formati di uscita (controllati dalle verifiche in
 * modulo8/verifiche/) sono descritti nei file dei passi.
 */

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

int main(void)
{
	/* Passo 2 e seguenti: inizializza i moduli qui. */

	printk("NODO AVVIATO\n");
	return 0;
}
