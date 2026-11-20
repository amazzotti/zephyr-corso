/*
 * Esercizio 9 (Modulo 6) - Riepilogo: termostato
 *
 * Teoria: teoria/07_periferiche_driver.md (tutto il modulo);
 *         teoria/05_sincronizzazione.md (mutex), teoria/06_tempo_lavoro_differito.md.
 *
 * OBIETTIVO
 * Un piccolo termostato di allarme che mette insieme sensore, pulsanti con interrupt e
 * debounce, LED e thread:
 *   - un thread legge la temperatura ogni secondo;
 *   - se la temperatura supera la soglia (in modalita' automatica) scatta l'allarme:
 *     led0 acceso e un messaggio; quando torna sotto, l'allarme rientra;
 *   - sw0 cambia modalita': automatica <-> manutenzione. In manutenzione l'allarme e'
 *     disattivato (led0 spento) e led1 e' acceso;
 *   - sw1 imposta la soglia a (temperatura attuale - 1.00 C): serve a provare
 *     l'allarme senza toccare il sensore;
 *   - sw2 imposta la soglia a (temperatura attuale + 1.00 C).
 *
 * PASSI (suggerimento di architettura: decidila tu, ma scrivila prima di programmare)
 * 1. Overlay con il nodo del BME280; gpio_dt_spec di led0, led1, sw0, sw1, sw2.
 * 2. Stato condiviso, protetto da un k_mutex:
 *        int32_t temperatura_c100;   (temperatura in centesimi di grado: 2534 = 25.34 C)
 *        int32_t soglia_c100 = 3000; (30.00 C)
 *        bool manutenzione = false;
 *        bool allarme = false;
 *    Lavorare in centesimi interi evita i numeri con la virgola. Conversione da
 *    sensor_value:  v.val1 * 100 + v.val2 / 10000.
 * 3. Una funzione stampa_c100(const char *prefisso, int32_t c100) che stampa
 *    "<prefisso>25.34 C" gestendo anche i negativi.
 * 4. Thread di acquisizione (K_THREAD_DEFINE, priorita' 5, stack 1024), ogni 1000 ms:
 *    fetch + get, aggiorna temperatura_c100, stampa "Temperatura: XX.XX C", poi chiama
 *    valuta_allarme():
 *      - in manutenzione: allarme = false, led0 spento;
 *      - altrimenti, se temperatura > soglia e non c'era allarme: allarme = true,
 *        led0 acceso, stampa "ALLARME: XX.XX C oltre la soglia";
 *      - se temperatura <= soglia e c'era allarme: allarme = false, led0 spento,
 *        stampa "Allarme rientrato".
 *    (Prima il LED, poi la stampa.)
 * 5. Pulsanti con interrupt + debounce (esercizio 3). Un'idea semplice: tutte e tre le
 *    callback fanno k_work_reschedule dello STESSO lavoro ritardato; il lavoro legge i
 *    tre pulsanti e confronta ciascuno con il suo stato stabile precedente (array).
 *    Alla pressione:
 *      - sw0: inverte manutenzione, imposta led1, chiama valuta_allarme() e stampa
 *        "Modalita: manutenzione" oppure "Modalita: automatica";
 *      - sw1 / sw2: nuova soglia = temperatura -/+ 100, stampa "Soglia: XX.XX C".
 * 6. main(): controlli dei device, configurazione dei pin e degli interrupt, poi
 *    stampa "Modalita: automatica" e la soglia iniziale ("Soglia: 30.00 C").
 *    I thread di acquisizione partono da soli; il main puo' terminare.
 * 7. Disegna (anche su carta) quali parti girano in interrupt, quali nella work queue
 *    e quali nel thread di acquisizione, e quali dati condividono. Scrivilo in fondo
 *    a questo commento.
 *
 * OUTPUT ATTESO (esempio, con il sensore simulato a 25 C)
 *     Modalita: automatica
 *     Soglia: 30.00 C
 *     Temperatura: 25.00 C
 *     Soglia: 24.00 C                         (sw1)
 *     ALLARME: 25.00 C oltre la soglia
 *     Temperatura: 25.00 C
 *     Soglia: 26.00 C                         (sw2)
 *     Allarme rientrato
 *     Modalita: manutenzione                  (sw0)
 *
 * CONCETTI NUOVI
 * - Nessuno: e' il riepilogo. Il punto e' far convivere interrupt, work queue, un
 *   thread periodico e dati condivisi senza race condition.
 *
 * COME VERIFICARE DA SOLO
 *     .\verifica.ps1 modulo6\09_riepilogo
 * La verifica usa sw1/sw2/sw0 e controlla messaggi e LED.
 *
 * ERRORI TIPICI
 * - k_mutex_lock nella callback del GPIO: in interrupt non si puo' (il lavoro ritardato
 *   gira invece in un thread, li' si puo').
 * - Stampare "ALLARME" a ogni lettura invece che solo al cambiamento.
 * - Leggere temperatura_c100 nel lavoro dei pulsanti senza mutex mentre il thread la
 *   scrive.
 * - Dimenticare di chiamare valuta_allarme() quando si entra in manutenzione: il LED
 *   resta acceso fino alla lettura successiva.
 *
 * BONUS (facoltativo)
 * - Isteresi: l'allarme rientra solo quando la temperatura scende sotto
 *   soglia - 0.50 C. Perche' serve? (Pensa a una temperatura che oscilla attorno alla
 *   soglia.) Ti servira' nel Modulo 8.
 * - Cambia la temperatura del sensore dal monitor di Renode (il comando trovato
 *   nell'esercizio 7) e osserva allarme e rientro.
 * - Fai lampeggiare led0 durante l'allarme invece di tenerlo fisso (k_timer).
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

/* TODO: device, stato condiviso, funzioni e thread (passi 1-5) */

int main(void)
{
	/* TODO: inizializzazione (passo 6) */

	return 0;
}
