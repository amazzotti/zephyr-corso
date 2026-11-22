/*
 * Esercizio 9 (Modulo 6) - Riepilogo: termostato
 *
 * Teoria: teoria/07_periferiche_driver.md (tutto il modulo);
 *         teoria/05_sincronizzazione.md (mutex, k_msgq), teoria/06_tempo_lavoro_differito.md.
 *
 * OBIETTIVO
 * Un piccolo termostato di allarme che mette insieme sensore, pulsanti a interrupt, LED
 * e thread:
 *   - un thread legge la temperatura ogni secondo (sensor API, esercizio 7);
 *   - se la temperatura supera la soglia (in modalita' automatica) scatta l'allarme:
 *     LED rosso (led2) acceso e un messaggio; quando torna sotto, l'allarme rientra;
 *   - sw0 (pulsante blu, PA0) cambia modalita': automatica <-> manutenzione.
 *     In manutenzione l'allarme e' disattivato (LED rosso spento) e il LED arancione
 *     (led1) e' acceso;
 *   - sw1 (PE7) imposta la soglia a (temperatura attuale - 1.00 C): serve a provare
 *     l'allarme senza cambiare la temperatura;
 *   - sw2 (PE8) imposta la soglia a (temperatura attuale + 1.00 C).
 * sw1 e sw2 sono pulsanti in piu' della scheda simulata, dichiarati in app.overlay.
 *
 * PASSI (suggerimento di architettura: decidila tu, ma scrivila prima di programmare)
 * 1. gpio_dt_spec di led1, led2, sw0, sw1, sw2 (DT_ALIAS(sw1) e DT_ALIAS(sw2) esistono
 *    grazie all'overlay); device del sensore DEVICE_DT_GET(DT_NODELABEL(sensore)).
 *    Controlli di tutti i device ("Errore: ... non pronto").
 * 2. Stato condiviso, protetto da un k_mutex:
 *        int32_t temperatura_c100;   (temperatura in centesimi di grado: 2534 = 25.34 C)
 *        int32_t soglia_c100 = 3000; (30.00 C)
 *        bool manutenzione = false;
 *        bool allarme = false;
 * 3. Una funzione stampa_c100(const char *prefisso, int32_t c100) che stampa
 *    "<prefisso>25.34 C" gestendo anche i negativi (esercizio 7).
 * 4. Thread di acquisizione (K_THREAD_DEFINE, priorita' 5, stack 1024), ogni 1000 ms:
 *    sensor_sample_fetch + sensor_channel_get (se fallisce: "Errore lettura: %d" e salta
 *    il giro), converte in centesimi, aggiorna temperatura_c100, stampa
 *    "Temperatura: XX.XX C", poi chiama valuta_allarme():
 *      - in manutenzione: allarme = false, LED rosso spento;
 *      - altrimenti, se temperatura > soglia e non c'era allarme: allarme = true,
 *        LED rosso acceso, stampa "ALLARME: XX.XX C oltre la soglia";
 *      - se temperatura <= soglia e c'era allarme: allarme = false, LED rosso spento,
 *        stampa "Allarme rientrato".
 *    (Prima il LED, poi la stampa.)
 * 5. Pulsanti a interrupt (esercizio 2): GPIO_INT_EDGE_TO_ACTIVE su tutti e tre.
 *    sw0 sta sulla porta A, sw1 e sw2 sulla porta E: servono due gpio_callback (una per
 *    porta; quella della porta E con la maschera BIT(sw1.pin) | BIT(sw2.pin), e nella
 *    callback guardi l'argomento "pins" per capire quale e' stato premuto).
 *    Le callback (interrupt!) NON gestiscono la pressione: mettono il numero del
 *    pulsante (0, 1, 2) in una k_msgq con K_NO_WAIT, e il main la svuota.
 * 6. main(): controlli, configurazione dei pin e delle callback, poi stampa
 *    "Modalita: automatica" e la soglia iniziale ("Soglia: 30.00 C"). Poi ciclo:
 *    k_msgq_get(&coda_pulsanti, &n, K_FOREVER) e, secondo il pulsante:
 *      - sw0: inverte manutenzione, imposta il LED arancione, chiama valuta_allarme() e
 *        stampa "Modalita: manutenzione" oppure "Modalita: automatica";
 *      - sw1 / sw2: nuova soglia = temperatura -/+ 100, stampa "Soglia: XX.XX C".
 * 7. Disegna (anche su carta) quali parti girano in interrupt, quali nel main e quali
 *    nel thread di acquisizione, e quali dati condividono. Scrivilo in fondo a questo
 *    commento.
 *
 * OUTPUT ATTESO (sensore simulato a 22.50 C, il valore iniziale)
 *     Modalita: automatica
 *     Soglia: 30.00 C
 *     Temperatura: 22.50 C
 *     Soglia: 21.50 C                         (sw1)
 *     ALLARME: 22.50 C oltre la soglia
 *     Temperatura: 22.50 C
 *     Soglia: 23.50 C                         (sw2)
 *     Allarme rientrato
 *     Modalita: manutenzione                  (sw0)
 *
 * CONCETTI NUOVI
 * - Nessuno: e' il riepilogo. Il punto e' far convivere interrupt, una coda, un thread
 *   periodico e dati condivisi senza race condition.
 * - Pulsanti su porte diverse = callback diverse; piu' pin della stessa porta = una
 *   callback con una maschera.
 *
 * COME VERIFICARE DA SOLO
 *     .\verifica.ps1 modulo6\09_riepilogo
 * La verifica usa sw1/sw2/sw0 e controlla messaggi e LED.
 *
 * ERRORI TIPICI
 * - k_mutex_lock in una callback del GPIO: in interrupt non si puo' (il main, che riceve
 *   la pressione dalla coda, e' un thread: li' si puo').
 * - Stampare "ALLARME" a ogni lettura invece che solo al cambiamento.
 * - Leggere temperatura_c100 nel main senza mutex mentre il thread la scrive.
 * - Dimenticare di chiamare valuta_allarme() quando si entra in manutenzione: il LED
 *   resta acceso fino alla lettura successiva.
 * - Una sola gpio_callback registrata su due porte diverse: una gpio_callback appartiene
 *   a UNA porta (gpio_add_callback(sw0.port, ...)).
 *
 * BONUS (facoltativo)
 * - Isteresi: l'allarme rientra solo quando la temperatura scende sotto
 *   soglia - 0.50 C. Perche' serve? (Pensa a una temperatura che oscilla attorno alla
 *   soglia.) Ti servira' nel Modulo 8.
 * - Cambia la temperatura dalla console (sim temp 3550, sim temp 2000) e osserva
 *   allarme e rientro.
 * - Debounce dei tre pulsanti (esercizio 3), con un lavoro ritardato per pulsante.
 * - Fai lampeggiare il LED rosso durante l'allarme invece di tenerlo fisso (k_timer).
 *
 * LA MIA ARCHITETTURA (passo 7):
 *   ...
 *
 * Scrivi il tuo codice qui sotto.
 */

#include <stdbool.h>
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/sys/printk.h>

/* TODO: device, stato condiviso, funzioni, callback e thread (passi 1-5) */

int main(void)
{
	/* TODO: inizializzazione e gestione dei pulsanti (passo 6) */

	return 0;
}
