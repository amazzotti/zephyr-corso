/*
 * Esercizio 6 (Modulo 1) - Enum e macchina a stati: il semaforo.
 *
 * Teoria: teoria/02_c_embedded.md, sezione 7.
 *
 * OBIETTIVO
 * Scrivere la prima macchina a stati, il modo piu' comune di organizzare la logica di
 * un firmware: un semaforo stradale che passa da ROSSO a VERDE a GIALLO e di nuovo a
 * ROSSO, restando in ogni stato per un tempo diverso.
 *
 * PASSI
 * 1. Definisci  enum stato_semaforo { STATO_ROSSO, STATO_VERDE, STATO_GIALLO };
 *    Stampa i valori numerici: "ROSSO=0 VERDE=1 GIALLO=2".
 * 2. Scrivi  const char *nome_stato(enum stato_semaforo s)  con uno switch che
 *    restituisce "ROSSO", "VERDE", "GIALLO" (e "?" nel default).
 * 3. Scrivi  uint32_t durata_ms(enum stato_semaforo s)  con uno switch:
 *    ROSSO 3000, VERDE 2000, GIALLO 1000.
 * 4. Scrivi  enum stato_semaforo prossimo(enum stato_semaforo s)  che restituisce lo
 *    stato successivo (ROSSO -> VERDE -> GIALLO -> ROSSO).
 * 5. In main: parti da STATO_ROSSO. Ripeti per 6 volte (2 cicli completi):
 *    - stampa "stato: ROSSO (3000 ms)"
 *    - aspetta la durata con k_msleep(durata)
 *    - passa allo stato successivo
 *    Alla fine stampa "fine dopo 2 cicli".
 * 6. Esegui con  .\esegui.ps1  e guarda i tempi tra una riga e l'altra.
 *
 * OUTPUT ATTESO
 *     ROSSO=0 VERDE=1 GIALLO=2
 *     stato: ROSSO (3000 ms)
 *     stato: VERDE (2000 ms)
 *     stato: GIALLO (1000 ms)
 *     stato: ROSSO (3000 ms)
 *     stato: VERDE (2000 ms)
 *     stato: GIALLO (1000 ms)
 *     fine dopo 2 cicli
 *
 * CONCETTI NUOVI
 * - enum: costanti intere con un nome (valgono 0, 1, 2... in ordine).
 * - switch / case / break / default.
 * - const char *: puntatore a una stringa costante (in flash).
 * - k_msleep(ms): il thread dorme per ms millisecondi; nel frattempo la CPU puo' fare
 *   altro (o dormire). Lo vedrai bene nel Modulo 3.
 * - Macchina a stati: stato attuale + regole di transizione.
 *
 * COME VERIFICARE DA SOLO
 *     .\verifica.ps1 modulo1\06_enum_stati
 *
 * ERRORI TIPICI
 * - Dimenticare break: il codice "cade" nel case successivo. GCC avvisa con
 *   -Wimplicit-fallthrough: leggi i warning.
 * - Senza default, se aggiungi uno stato all'enum e dimentichi di gestirlo, la funzione
 *   non restituisce niente di sensato. Il warning -Wswitch ti aiuta: prova a togliere
 *   un case e guarda cosa dice il compilatore.
 * - Usare numeri (0, 1, 2) invece dei nomi dell'enum: funziona, ma il codice diventa
 *   illeggibile e fragile.
 *
 * BONUS (facoltativo)
 * - Aggiungi lo stato STATO_LAMPEGGIANTE (giallo lampeggiante, "notte") dopo 2 cicli.
 * - Al posto delle due funzioni con switch, usa una TABELLA: un array di struct
 *   { const char *nome; uint32_t durata_ms; } indicizzato con lo stato. Quale soluzione
 *   ti sembra piu' facile da estendere?
 * - Stampa anche k_uptime_get_32() a ogni transizione per misurare i tempi.
 *
 * Scrivi il tuo codice qui sotto.
 */

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <stdint.h>

/* TODO passo 1: enum stato_semaforo */

/* TODO passi 2-4: nome_stato, durata_ms, prossimo */

int main(void)
{
	/* TODO passo 5 */

	return 0;
}
