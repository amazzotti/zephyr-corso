/*
 * Esercizio 9 (Modulo 6) - Riepilogo: termostato
 *
 * Teoria: teoria/07_periferiche_driver.md (tutto il modulo);
 *         teoria/05_sincronizzazione.md (mutex), teoria/06_tempo_lavoro_differito.md.
 *
 * OBIETTIVO
 * Un piccolo termostato di allarme che mette insieme sensore, pulsanti campionati con
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
 * 0. Prerequisito: l'esercizio 7 deve essere svolto. Questo esercizio compila il TUO
 *    driver da ../07_driver_bme280/src (vedi CMakeLists.txt): niente copie.
 * 1. gpio_dt_spec di led0, led1, sw0, sw1, sw2; in main() bme280c_init() (se fallisce:
 *    "Errore init sensore: %d").
 * 2. Stato condiviso, protetto da un k_mutex:
 *        int32_t temperatura_c100;   (temperatura in centesimi di grado: 2534 = 25.34 C)
 *        int32_t soglia_c100 = 3000; (30.00 C)
 *        bool manutenzione = false;
 *        bool allarme = false;
 *    Lavorare in centesimi interi evita i numeri con la virgola: il tuo driver li
 *    restituisce gia' cosi' (bme280c_leggi_centesimi).
 * 3. Una funzione stampa_c100(const char *prefisso, int32_t c100) che stampa
 *    "<prefisso>25.34 C" gestendo anche i negativi.
 * 4. Thread di acquisizione (K_THREAD_DEFINE, priorita' 5, stack 1024), ogni 1000 ms:
 *    bme280c_leggi_centesimi (se fallisce: "Errore lettura: %d" e salta il giro),
 *    aggiorna temperatura_c100, stampa "Temperatura: XX.XX C", poi chiama
 *    valuta_allarme():
 *      - in manutenzione: allarme = false, led0 spento;
 *      - altrimenti, se temperatura > soglia e non c'era allarme: allarme = true,
 *        led0 acceso, stampa "ALLARME: XX.XX C oltre la soglia";
 *      - se temperatura <= soglia e c'era allarme: allarme = false, led0 spento,
 *        stampa "Allarme rientrato".
 *    (Prima il LED, poi la stampa.)
 * 5. Pulsanti campionati con debounce (esercizi 2 e 3): un solo k_timer ogni 10 ms
 *    legge i tre pulsanti e applica a ciascuno il debounce per campionamento (array di
 *    stati: stabile, candidato, uguali). Quando un pulsante diventa stabilmente premuto,
 *    la funzione del timer (interrupt!) NON gestisce la pressione: mette l'indice del
 *    pulsante in una k_msgq (o fa k_work_submit) e un thread/lavoro fa il resto.
 *    Alla pressione:
 *      - sw0: inverte manutenzione, imposta led1, chiama valuta_allarme() e stampa
 *        "Modalita: manutenzione" oppure "Modalita: automatica";
 *      - sw1 / sw2: nuova soglia = temperatura -/+ 100, stampa "Soglia: XX.XX C".
 * 6. main(): controlli dei device, configurazione dei pin, avvio del timer dei
 *    pulsanti, poi stampa "Modalita: automatica" e la soglia iniziale
 *    ("Soglia: 30.00 C"). I thread partono da soli; il main puo' terminare (o fare
 *    lui da thread che gestisce le pressioni lette dalla coda).
 * 7. Disegna (anche su carta) quali parti girano in interrupt (timer), quali nella
 *    work queue o nel thread dei pulsanti e quali nel thread di acquisizione, e quali
 *    dati condividono. Scrivilo in fondo a questo commento.
 *
 * OUTPUT ATTESO (con il sensore simulato a 22.50 C, il valore iniziale in Renode)
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
 * - Nessuno: e' il riepilogo. Il punto e' far convivere un timer (interrupt), una coda
 *   o work queue, un thread periodico e dati condivisi senza race condition.
 *
 * COME VERIFICARE DA SOLO
 *     .\verifica.ps1 modulo6\09_riepilogo
 * La verifica usa sw1/sw2/sw0 e controlla messaggi e LED.
 *
 * ERRORI TIPICI
 * - k_mutex_lock nella funzione del timer: in interrupt non si puo' (il thread o il
 *   lavoro della work queue che riceve la pressione gira invece in un thread, li' si
 *   puo').
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
 * - Cambia la temperatura del sensore dal monitor di Renode
 *   (sysbus.twi0.bme280 Temperature 35.5) e osserva allarme e rientro.
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
#include <zephyr/sys/printk.h>

#include "bme280_corso.h"   /* il tuo driver, dall'esercizio 7 */

/* TODO: device, stato condiviso, funzioni e thread (passi 1-5) */

int main(void)
{
	/* TODO: inizializzazione (passo 6) */

	return 0;
}
