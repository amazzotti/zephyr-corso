/*
 * Esercizio 6 (Modulo 6) - I2C a basso livello
 *
 * Teoria: teoria/07_periferiche_driver.md, sezioni 7 e 8 (e il riquadro
 *         "Limite di Renode" nella sezione 7);
 *         teoria/02_c_embedded.md (operatori sui bit, tipi con segno).
 *
 * OBIETTIVO
 * Parlare con il sensore BME280 direttamente sul bus I2C, come farebbe un driver:
 * leggere il registro di identificazione, poi i primi coefficienti di calibrazione,
 * e ricomporli da byte a numeri a 16 bit.
 *
 * PASSI
 * 1. Prendi il device del bus e definisci l'indirizzo del sensore:
 *        static const struct device *bus = DEVICE_DT_GET(DT_NODELABEL(i2c0));
 *        #define BME280_ADDR 0x76
 *    In main() controlla device_is_ready(bus) ("Errore: bus I2C non pronto") e stampa
 *        "Bus %s, indirizzo 0x%02x"   con bus->name e BME280_ADDR.
 *    (Il bus i2c0 e' gia' attivo nel devicetree della nrf52840dk: niente overlay.)
 * 2. Scrivi la funzione di aiuto
 *        static int leggi_registri(uint8_t reg, uint8_t *buf, size_t n)
 *    che legge n registri consecutivi a partire da "reg" in DUE transazioni:
 *        - i2c_write(bus, &reg, 1, BME280_ADDR)   -> dice al sensore da che registro partire
 *        - i2c_read(bus, buf, n, BME280_ADDR)     -> legge n byte da li' in avanti
 *    Se una delle due restituisce un valore negativo, restituiscilo subito; altrimenti 0.
 *    LIMITE DI RENODE: sulla scheda vera si userebbe una sola chiamata,
 *        i2c_write_read(bus, BME280_ADDR, &reg, 1, buf, n)   (o i2c_burst_read),
 *    che fa scrittura e lettura nella STESSA transazione, con un "repeated start" al posto
 *    dello STOP in mezzo. Il modello I2C del simulatore non gestisce il repeated start
 *    (restituisce dati sbagliati senza errori!), mentre gestisce bene due transazioni
 *    separate. Il BME280 accetta entrambe le forme.
 * 3. Leggi 1 byte dal registro 0xD0. Se leggi_registri restituisce un valore negativo
 *    stampa "Errore I2C: %d" (significa: nessun ACK, il sensore non risponde a
 *    quell'indirizzo) e termina. Altrimenti stampa "ID chip: 0x%02x".
 *    Se id == 0x60 stampa "BME280 trovato", altrimenti "Chip sconosciuto".
 * 4. Leggi 6 byte a partire dal registro 0x88 in un colpo solo:
 *        uint8_t cal[6];
 *        leggi_registri(0x88, cal, sizeof(cal));
 *    Stampali in esadecimale su una riga ("Calibrazione: 00 80 ...").
 * 5. Ricomponi i coefficienti (little-endian: prima il byte basso):
 *        uint16_t dig_T1 = cal[0] | (cal[1] << 8);
 *        int16_t  dig_T2 = ...  (byte 2 e 3)
 *        int16_t  dig_T3 = ...  (byte 4 e 5)
 *    e stampa "dig_T1 = %u, dig_T2 = %d, dig_T3 = %d".
 * 6. Cambia BME280_ADDR in 0x77, ricompila ed esegui: cosa succede al passo 3?
 *    Rimetti 0x76.
 *
 * OUTPUT ATTESO (valori del sensore simulato da Renode: i coefficienti di un BME280 vero
 * sono diversi per ogni esemplare)
 *     Bus i2c@40003000, indirizzo 0x76
 *     ID chip: 0x60
 *     BME280 trovato
 *     Calibrazione: 00 80 00 80 00 00
 *     dig_T1 = 32768, dig_T2 = -32768, dig_T3 = 0
 *
 * CONCETTI NUOVI
 * - Device di un bus: DEVICE_DT_GET(DT_NODELABEL(i2c0)); il dispositivo sul bus si
 *   identifica con il suo indirizzo a 7 bit.
 * - Registro di un dispositivo: "scrivo il numero del registro, poi leggo".
 * - Lettura a raffica (burst): il sensore avanza da solo al registro successivo.
 * - ACK/NACK: se nessuno risponde all'indirizzo la funzione restituisce un errore.
 * - Little-endian e ricomposizione di numeri da byte; con segno contro senza segno.
 * - Repeated start (teoria, sezione 7): perche' esiste e perche' qui non lo usiamo.
 *
 * COME VERIFICARE DA SOLO
 *     .\verifica.ps1 modulo6\06_i2c_registri
 *
 * ERRORI TIPICI
 * - Usare i2c_write_read o i2c_reg_read_byte: in Renode leggi valori sbagliati (per
 *   esempio un ID diverso da 0x60) SENZA errori. Vedi il riquadro al passo 2.
 * - Passare a i2c_write il valore del registro invece del suo indirizzo: e' &reg, un
 *   puntatore al buffer da trasmettere (lungo 1 byte).
 * - Ricomporre al contrario (cal[0] << 8 | cal[1]): big-endian, valori assurdi.
 * - dig_T2 come uint16_t: i valori negativi diventano numeri enormi (32768 invece di
 *   -32768).
 * - Dimenticare le parentesi: cal[0] | cal[1] << 8 funziona, ma (cal[0] | cal[1]) << 8 no.
 *
 * BONUS (facoltativo)
 * - Nel monitor di Renode scrivi  logLevel 0 sysbus.twi0  e riesegui: vedi le due
 *   transazioni (scrittura del registro, poi lettura) sul bus?
 * - Prova una volta i2c_write_read al posto delle due chiamate e confronta l'ID letto:
 *   e' il modo migliore per ricordarti del limite del simulatore.
 * - Una misura vera la fai nell'esercizio 7, dove scrivi un piccolo driver completo.
 *
 * Scrivi il tuo codice qui sotto.
 */

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/sys/printk.h>

/* TODO: device del bus e indirizzo del sensore (passo 1) */

/* TODO: static int leggi_registri(uint8_t reg, uint8_t *buf, size_t n)  (passo 2) */

int main(void)
{
	/* TODO: lettura dell'ID e della calibrazione (passi 1, 3-5) */

	return 0;
}
