/*
 * Esercizio 6 (Modulo 6) - I2C a basso livello: i registri del sensore
 *
 * Teoria: teoria/07_periferiche_driver.md, sezioni 7 e 8.
 *
 * OBIETTIVO
 * Parlare con un sensore di temperatura I2C (Analog Devices ADT7420, indirizzo 0x48)
 * leggendo direttamente i suoi registri: l'identificativo, la configurazione e la
 * temperatura grezza, che poi converti tu in gradi. Niente driver: solo bus I2C.
 *
 * Il sensore e' simulato da Zephyr (cartella comune/, vedi CORSO.md sezione 3): per il
 * tuo codice e' un normale dispositivo I2C. La temperatura parte da 22.50 C e si cambia
 * scrivendo nella console  sim temp 3550  (centesimi di grado).
 *
 * PASSI
 * 1. Prendi bus + indirizzo dal devicetree, in un solo oggetto:
 *        static const struct i2c_dt_spec sens = I2C_DT_SPEC_GET(DT_NODELABEL(sensore));
 *    e controlla i2c_is_ready_dt(&sens) ("Errore: bus I2C non pronto").
 *    Cerca "sensore" in build/zephyr/zephyr.dts: sotto quale bus sta? Che "reg" ha?
 * 2. Scrivi una funzione che legge uno o piu' registri consecutivi:
 *        static int leggi_registri(uint8_t reg, uint8_t *buf, size_t n)
 *        {
 *            return i2c_write_read_dt(&sens, &reg, 1, buf, n);
 *        }
 *    i2c_write_read_dt fa in una sola transazione: START, indirizzo + scrittura, numero
 *    del registro, RESTART (repeated start), indirizzo + lettura, n byte, STOP.
 * 3. Leggi il registro 0x0B (ID) e stampa  "ID: 0x%02x"  -> deve valere 0xcb.
 *    Se la funzione restituisce un errore stampa "Errore I2C: %d" e termina.
 * 4. Leggi il registro 0x03 (configurazione) e stampa  "Configurazione: 0x%02x".
 *    Con la tabella della teoria (sezione 8): che risoluzione e che modo di
 *    funzionamento indica?
 * 5. Ogni secondo, in un ciclo:
 *      - leggi 2 byte dal registro 0x00 (temperatura: MSB poi LSB);
 *      - ricomponi il valore a 16 bit: (msb << 8) | lsb, in un int16_t (ha il segno!);
 *      - i 13 bit utili stanno nei bit 15..3: grezzo = valore >> 3;
 *      - ogni unita' vale 0,0625 C = 1/16 C. In centesimi di grado, senza float:
 *            centesimi = grezzo * 25 / 4;     (perche'? 100/16 = 25/4)
 *      - stampa  "Grezzo: %d -> %d.%02d C"  (attenzione ai negativi, bonus).
 * 6. Esegui, poi nella finestra della UART scrivi  sim temp 3550  e guarda cambiare i
 *    valori. Prova anche  sim temp 2000.
 *
 * OUTPUT ATTESO
 *     ID: 0xcb
 *     Configurazione: 0x00
 *     Grezzo: 360 -> 22.50 C
 *     Grezzo: 360 -> 22.50 C
 *     sim: temperatura 35.50 C            (dopo "sim temp 3550")
 *     Grezzo: 568 -> 35.50 C
 * (Le righe possono iniziare con il prompt "uart:~$ " della shell: e' normale.)
 *
 * CONCETTI NUOVI
 * - Indirizzo I2C a 7 bit del dispositivo (0x48) e "registri" del dispositivo:
 *   si scrive il numero del registro, poi si legge.
 * - Repeated start: scrittura e lettura nella stessa transazione, senza STOP in mezzo.
 * - Big-endian: l'ADT7420 manda prima il byte ALTO (MSB). Ricomporre a mano i byte.
 * - Complemento a due e shift a destra di un numero con segno (Modulo 1).
 * - Virgola fissa: centesimi di grado come intero.
 *
 * COME VERIFICARE DA SOLO
 *     .\verifica.ps1 modulo6\06_i2c_registri
 * La verifica controlla ID, temperatura iniziale e temperatura dopo "sim temp 3550".
 *
 * ERRORI TIPICI
 * - Ricomporre i byte al contrario (lsb << 8 | msb): con 22.50 C esce un numero enorme.
 * - Usare uint16_t: con temperature negative il valore diventa sbagliato (bonus).
 * - Dimenticare lo shift di 3 bit: 22.50 C diventa 180.00 C.
 * - Calcolare 25/4 prima della moltiplicazione: in intero 25/4 = 6, e si perdono i
 *   decimali. Prima moltiplica, poi dividi.
 *
 * BONUS (facoltativo)
 * - Temperature negative: sim temp -525 deve stampare "Grezzo: -84 -> -5.25 C".
 *   Attenzione a "-0.50": gradi = 0 ma il numero e' negativo. Scrivi una funzione
 *   stampa_c100(int32_t c100) che gestisce il segno a parte.
 * - Prova un indirizzo sbagliato (0x49 al posto di 0x48, con una i2c_dt_spec scritta a
 *   mano): che errore restituisce i2c_write_read_dt? Cosa succede sul bus (NACK)?
 * - Leggi lo stato (registro 0x02): cosa indica il bit 7 (RDY)?
 *
 * Scrivi il tuo codice qui sotto.
 */

#include <zephyr/kernel.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/sys/printk.h>

#define REG_TEMP_MSB 0x00
#define REG_CONFIG   0x03
#define REG_ID       0x0B

/* TODO: i2c_dt_spec del sensore e funzione leggi_registri (passi 1-2) */

int main(void)
{
	/* TODO: ID, configurazione e ciclo di lettura della temperatura (passi 3-5) */

	return 0;
}
