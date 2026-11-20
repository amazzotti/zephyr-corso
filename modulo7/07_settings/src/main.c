/*
 * Esercizio 7 (Modulo 7) - Salvare la configurazione
 *
 * Teoria: teoria/08_strumenti_debug_test.md, sezione 7.
 *
 * OBIETTIVO
 * Salvare dei valori in flash con il sottosistema settings e rileggerli:
 *     corso/periodo   un periodo di lampeggio in ms (uint32_t), predefinito 1000
 *     corso/avvii     quante volte il firmware e' partito (uint32_t)
 *
 * In Renode la flash e' SIMULATA in RAM (vedi app.overlay e la teoria): il codice e'
 * identico a quello per la scheda vera, ma i dati si perdono con "machine Reset".
 * Per questo il "riavvio" lo simuliamo dentro il programma (passo 4).
 *
 * PASSI
 * 1. Variabili globali: periodo_ms = 1000 e avvii = 0 (uint32_t).
 * 2. Scrivi l'handler "set" per il prefisso "corso" (esempio nella teoria):
 *        static int corso_set(const char *nome, size_t len,
 *                             settings_read_cb leggi, void *arg)
 *    Se nome e' "periodo" legge in periodo_ms, se e' "avvii" in avvii; controlla che
 *    len == sizeof(uint32_t). Registralo con SETTINGS_STATIC_HANDLER_DEFINE.
 * 3. In main:
 *    a. settings_subsys_init(), poi settings_load(). Controlla i valori di ritorno
 *       (negativo = errore: stampalo e fermati).
 *    b. avvii++, salvalo con settings_save_one("corso/avvii", &avvii, sizeof(avvii)),
 *       stampa "Avvio n. N".
 *    c. Stampa "Periodo all'avvio = P ms".
 *    d. Imposta periodo_ms = 250, salvalo, stampa "Salvo periodo = 250 ms".
 * 4. Simula il riavvio: periodo_ms = 0, stampa "Simulo un riavvio: periodo azzerato",
 *    chiama di nuovo settings_load() e stampa "Ricaricato periodo = P ms".
 *    Deve tornare 250: il valore e' arrivato dalla flash (simulata).
 * 5. Esegui, poi nel monitor di Renode "machine Reset" e "start": quanto vale "Avvio n."?
 *    Perche'? (Domanda 2)
 *
 * OUTPUT ATTESO
 *     Avvio n. 1
 *     Periodo all'avvio = 1000 ms
 *     Salvo periodo = 250 ms
 *     Simulo un riavvio: periodo azzerato
 *     Ricaricato periodo = 250 ms
 *
 * CONCETTI NUOVI
 * - Flash: si scrive (1 -> 0) a blocchi, si cancella a pagine intere, si consuma.
 * - NVS: file system chiave-valore che gestisce pagine e usura per te.
 * - settings: valori con un nome ("corso/periodo"); handler per prefisso chiamati da
 *   settings_load per ogni valore trovato.
 * - settings_read_cb: la funzione che ti viene passata per leggere il valore; restituisce
 *   quanti byte ha letto.
 * - Overlay di configurazione: lo stesso codice gira su flash vera o simulata.
 *
 * COME VERIFICARE DA SOLO
 *     .\verifica.ps1 modulo7\07_settings
 *
 * ERRORI TIPICI
 * - Dimenticare settings_subsys_init(): settings_save_one fallisce con errore.
 * - Nome sbagliato nell'handler: il prefisso "corso/" NON arriva all'handler, arriva solo
 *   "periodo" (usa strcmp o settings_name_steq).
 * - Non controllare len: un valore salvato con un tipo diverso verrebbe letto a meta'.
 * - Salvare in un ciclo veloce: sulla flash vera consumi le pagine (qui no, ma e' un vizio
 *   da non prendere).
 *
 * BONUS (facoltativo)
 * - Aggiungi una stringa "corso/nome" (char[16]); attento a len e al terminatore.
 * - settings_delete("corso/periodo") e poi settings_load: cosa succede a periodo_ms?
 * - Salva 1000 volte lo stesso valore e guarda con LOG/printk quanto ci mette: NVS
 *   scrive davvero ogni volta? (Cerca CONFIG_SETTINGS_NVS / "duplicate" nella documentazione.)
 *
 * DOMANDE (rispondi qui)
 * 1. Chi chiama corso_set, e quante volte durante settings_load?
 * 2. Dopo machine Reset, "Avvio n." vale 1 o 2? Perche'? Sulla scheda vera?
 *
 * Scrivi il tuo codice qui sotto.
 */

#include <string.h>
#include <zephyr/kernel.h>
#include <zephyr/settings/settings.h>
#include <zephyr/sys/printk.h>

/* TODO: variabili periodo_ms e avvii. */

/* TODO: handler corso_set e SETTINGS_STATIC_HANDLER_DEFINE. */

int main(void)
{
	/* TODO: passi 3 e 4. */
	return 0;
}
