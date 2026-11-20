/*
 * Esercizio 5 (Modulo 1) - Struct.
 *
 * Teoria: teoria/02_c_embedded.md, sezioni 5 e 6.
 *
 * OBIETTIVO
 * Raggruppare dati in una struct (la "misura" di un sensore, che ritroverai nel progetto
 * finale), usarla con array e puntatori, e scoprire il padding: perche' una struct
 * occupa piu' byte della somma dei suoi campi.
 *
 * PASSI
 * 1. La struct e' gia' definita qui sotto (struct misura). Definisci anche
 *        struct misura_disordinata { uint8_t sensore_id; uint32_t timestamp_ms;
 *                                    int16_t temperatura_decimi; };
 *    (stessi campi, ordine diverso) e stampa
 *        sizeof(struct misura) = 8
 *        sizeof(struct misura_disordinata) = 12
 *    I dati sono 7 byte in entrambi i casi: disegna su carta dove va il padding.
 * 2. Crea un array di 3 misure con gli inizializzatori "designati" (.campo = valore):
 *        { .timestamp_ms = 1000, .temperatura_decimi = 215, .sensore_id = 1 }
 *        { .timestamp_ms = 2000, .temperatura_decimi = 230, .sensore_id = 1 }
 *        { .timestamp_ms = 3000, .temperatura_decimi = 198, .sensore_id = 2 }
 * 3. Scrivi  void stampa_misura(const struct misura *m)  che usa -> e stampa
 *        t=1000 ms sensore 1: 21.5 C
 *    (temperatura in virgola fissa: decimi / 10 e decimi % 10). Chiamala sulle 3 misure.
 * 4. Scrivi  int32_t media_decimi(const struct misura *v, size_t n)  e stampa
 *        media = 21.4 C
 *    (643 / 3 = 214 con la divisione intera).
 * 5. Per valore o per puntatore? Scrivi due funzioni che aggiungono delta alla temperatura:
 *        void scalda_copia(struct misura m, int16_t delta)
 *        void scalda(struct misura *m, int16_t delta)
 *    Prendi una copia della prima misura (struct misura prova = elenco[0];),
 *    chiama scalda_copia(prova, 10) e stampa "per valore: 21.5 C" (non e' cambiata!),
 *    poi scalda(&prova, 10) e stampa "per puntatore: 22.5 C".
 *    Per queste due righe stampa solo la temperatura, nello stesso formato "xx.y C".
 *
 * OUTPUT ATTESO
 *     sizeof(struct misura) = 8
 *     sizeof(struct misura_disordinata) = 12
 *     t=1000 ms sensore 1: 21.5 C
 *     t=2000 ms sensore 1: 23.0 C
 *     t=3000 ms sensore 2: 19.8 C
 *     media = 21.4 C
 *     per valore: 21.5 C
 *     per puntatore: 22.5 C
 *
 * CONCETTI NUOVI
 * - struct: tipo composto. Accesso con . (variabile) e -> (puntatore).
 * - Inizializzatori designati: { .campo = valore }; i campi non citati valgono 0.
 * - Assegnare una struct a un'altra copia tutti i byte (struct misura b = a;).
 * - Passare una struct per valore la copia: la funzione lavora su una copia.
 * - Padding e allineamento: un uint32_t deve stare a un indirizzo multiplo di 4.
 *
 * COME VERIFICARE DA SOLO
 *     .\verifica.ps1 modulo1\05_struct
 *
 * ERRORI TIPICI
 * - m.temperatura_decimi con m puntatore: errore "request for member in something not
 *   a structure"; serve m->temperatura_decimi.
 * - Dimenticare "struct" davanti al nome del tipo (in C serve, a meno di typedef).
 * - Stampare 23.0 come "23.0" richiede %d.%d con 230 / 10 e 230 % 10: funziona.
 *   Ma con temperature NEGATIVE (-15 decimi) / e % danno -1 e -5: "-1.-5"! Qui non
 *   succede, ma ricordatelo per il progetto finale.
 *
 * BONUS (facoltativo)
 * - Usa offsetof(struct misura, sensore_id) (da <stddef.h>) per stampare la posizione
 *   di ogni campo nelle due struct e verificare il tuo disegno del padding.
 * - Scrivi una stampa corretta anche per temperature negative (suggerimento: segno a
 *   parte e valore assoluto).
 * - typedef: definisci  typedef struct misura misura_t;  e usa misura_t.
 *
 * Scrivi il tuo codice qui sotto.
 */

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <stdint.h>
#include <stddef.h>

struct misura {
	uint32_t timestamp_ms;      /* quando: millisecondi dall'avvio */
	int16_t temperatura_decimi; /* quanto: 215 = 21.5 gradi */
	uint8_t sensore_id;         /* chi: numero del sensore */
};

/* TODO passo 1: struct misura_disordinata */

/* TODO passo 3: void stampa_misura(const struct misura *m) */

/* TODO passo 4: int32_t media_decimi(const struct misura *v, size_t n) */

/* TODO passo 5: scalda_copia e scalda */

int main(void)
{
	/* TODO passi 1-5 */

	return 0;
}
