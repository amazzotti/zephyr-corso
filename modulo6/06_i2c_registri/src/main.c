/*
 * Esercizio 6 (Modulo 6) - I2C a basso livello
 *
 * Teoria: teoria/07_periferiche_driver.md, sezioni 7 e 8;
 *         teoria/02_c_embedded.md (operatori sui bit, tipi con segno).
 *
 * OBIETTIVO
 * Parlare con il sensore BME280 direttamente sul bus I2C, come farebbe un driver:
 * leggere il registro di identificazione, poi i primi coefficienti di calibrazione,
 * e ricomporli da byte a numeri a 16 bit.
 *
 * PASSI
 * 1. In app.overlay aggiungi il nodo del sensore sotto &i2c0 (teoria, sezione 7):
 *    etichetta bme280, compatible "bosch,bme280", reg = <0x76>, e status = "okay"
 *    sul bus. Compila e cerca il nodo in build/zephyr/zephyr.dts.
 * 2. Prendi la i2c_dt_spec del sensore:
 *        static const struct i2c_dt_spec sens = I2C_DT_SPEC_GET(DT_NODELABEL(bme280));
 *    e controlla i2c_is_ready_dt(&sens) ("Errore: bus I2C non pronto").
 *    Stampa "Bus %s, indirizzo 0x%02x" con sens.bus->name e sens.addr.
 * 3. Leggi il registro 0xD0 con i2c_reg_read_byte_dt(&sens, 0xD0, &id). Se il valore
 *    restituito e' negativo stampa "Errore I2C: %d" (significa: nessun ACK, il sensore
 *    non risponde a quell'indirizzo). Altrimenti stampa "ID chip: 0x%02x".
 *    Se id == 0x60 stampa "BME280 trovato", altrimenti "Chip sconosciuto".
 * 4. Leggi 6 byte a partire dal registro 0x88 in un colpo solo:
 *        uint8_t cal[6];
 *        i2c_burst_read_dt(&sens, 0x88, cal, sizeof(cal));
 *    Stampali in esadecimale su una riga ("Calibrazione: 6e 6b ...").
 * 5. Ricomponi i coefficienti (little-endian: prima il byte basso):
 *        uint16_t dig_T1 = cal[0] | (cal[1] << 8);
 *        int16_t  dig_T2 = ...  (byte 2 e 3)
 *        int16_t  dig_T3 = ...  (byte 4 e 5)
 *    e stampa "dig_T1 = %u, dig_T2 = %d, dig_T3 = %d".
 * 6. Prova a cambiare l'indirizzo nell'overlay in 0x77, ricompila (-Pulito) ed esegui:
 *    cosa succede al passo 3? Rimetti 0x76.
 *
 * OUTPUT ATTESO (i valori di calibrazione dipendono dal modello simulato)
 *     Bus i2c@40003000, indirizzo 0x76
 *     ID chip: 0x60
 *     BME280 trovato
 *     Calibrazione: xx xx xx xx xx xx
 *     dig_T1 = NNNNN, dig_T2 = NNNNN, dig_T3 = NN
 *
 * CONCETTI NUOVI
 * - Nodo figlio di un bus nel devicetree: reg = indirizzo I2C a 7 bit.
 * - struct i2c_dt_spec: bus + indirizzo; I2C_DT_SPEC_GET e i2c_is_ready_dt.
 * - Registro di un dispositivo: "scrivo il numero del registro, poi leggo".
 * - Lettura a raffica (burst): il sensore avanza da solo al registro successivo.
 * - ACK/NACK: se nessuno risponde all'indirizzo la funzione restituisce un errore.
 * - Little-endian e ricomposizione di numeri da byte; con segno contro senza segno.
 *
 * COME VERIFICARE DA SOLO
 *     .\verifica.ps1 modulo6\06_i2c_registri
 *
 * ERRORI TIPICI
 * - Overlay con errori di sintassi: manca ";" dopo "}" o dopo una proprieta'. L'errore
 *   arriva da dtc con il numero di riga dell'overlay.
 * - Nodo senza etichetta "bme280:" davanti: DT_NODELABEL(bme280) non lo trova
 *   (errore lungo con "DT_N_NODELABEL_bme280").
 * - Ricomporre al contrario (cal[0] << 8 | cal[1]): big-endian, valori assurdi.
 * - dig_T2 come uint16_t: i valori negativi diventano numeri enormi.
 * - Dimenticare le parentesi: cal[0] | cal[1] << 8 funziona, ma (cal[0] | cal[1]) << 8 no.
 *
 * BONUS (facoltativo)
 * - Una misura vera: scrivi 0x25 nel registro 0xF4 (ctrl_meas: temperatura x1,
 *   pressione x1, modo forced), aspetta 10 ms, leggi 3 byte da 0xFA e calcola
 *       adc_T = (msb << 12) | (lsb << 4) | (xlsb >> 4)
 *   poi applica la formula di compensazione del datasheet:
 *       var1 = ((((adc_T >> 3) - ((int32_t)dig_T1 << 1))) * dig_T2) >> 11;
 *       var2 = (((((adc_T >> 4) - dig_T1) * ((adc_T >> 4) - dig_T1)) >> 12) * dig_T3) >> 14;
 *       T = ((var1 + var2) * 5 + 128) >> 8;      (centesimi di grado)
 *   (tutte le variabili int32_t). Stampa la temperatura come "%d.%02d C".
 * - Nel monitor di Renode prova  logLevel 0 sysbus.twi0  e riesegui: vedi le
 *   transazioni sul bus?
 *
 * Scrivi il tuo codice qui sotto.
 */

#include <zephyr/kernel.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/sys/printk.h>

/* TODO: i2c_dt_spec del sensore (passo 2, dopo aver scritto l'overlay) */

int main(void)
{
	/* TODO: lettura dell'ID e della calibrazione (passi 2-5) */

	return 0;
}
