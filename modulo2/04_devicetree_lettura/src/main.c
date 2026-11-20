/*
 * Esercizio 4 (Modulo 2) - Leggere il devicetree
 *
 * Teoria: teoria/03_build_kconfig_devicetree.md, sezioni 5 e 6.
 *
 * OBIETTIVO
 * Imparare a leggere la "mappa dell'hardware" della scheda: trovare i nodi in
 * build/zephyr/zephyr.dts e leggerne le proprieta' dal codice con le macro DT_...
 * Collegare quello che vedi con i registri del chip e con Renode.
 *
 * PASSI
 * 1. Compila lo scheletro (.\esegui.ps1 modulo2\04_devicetree_lettura), poi apri
 *    build/zephyr/zephyr.dts e trova:
 *      - il nodo "chosen" e la proprieta' zephyr,console: a quale UART punta?
 *      - il nodo uart0 (uart@40002000): proprieta' reg e current-speed;
 *      - il nodo leds e il figlio led_0: proprieta' gpios (porta, pin, flag);
 *      - i nodi puntati da zephyr,flash e zephyr,sram: proprieta' reg;
 *      - il nodo i2c0.
 *    Rispondi alle domande in risposte.md.
 * 2. Nel codice stampa, con le macro della teoria (sezione 6):
 *      "Velocita console: %d"   DT_PROP(DT_CHOSEN(zephyr_console), current_speed)
 *      "Indirizzo uart0: 0x%08x"  DT_REG_ADDR(DT_NODELABEL(uart0))
 *      "LED0 sul pin: %d"       DT_GPIO_PIN(DT_ALIAS(led0), gpios)
 *      "Flash: %d byte"         DT_REG_SIZE(DT_CHOSEN(zephyr_flash))
 *      "RAM: %d byte"           DT_REG_SIZE(DT_CHOSEN(zephyr_sram))
 *      "Indirizzo i2c0: 0x%08x"   DT_REG_ADDR(DT_NODELABEL(i2c0))
 *    Gli indirizzi conviene convertirli: (uint32_t)DT_REG_ADDR(...) e stamparli con
 *    %08x (8 cifre esadecimali con zeri davanti).
 * 3. In Renode scrivi  peripherals  nel monitor: trovi gli stessi indirizzi di uart0
 *    e di i2c0 (in Renode si chiama twi0)? Il devicetree descrive esattamente
 *    l'hardware che Renode simula.
 * 4. Lancia la verifica.
 *
 * OUTPUT ATTESO
 *     Velocita console: 115200
 *     Indirizzo uart0: 0x40002000
 *     LED0 sul pin: 13
 *     Flash: 1048576 byte
 *     RAM: 262144 byte
 *     Indirizzo i2c0: 0x40003000
 *
 * CONCETTI NUOVI
 * - Devicetree: descrizione dell'hardware (nodi, proprieta', etichette, alias, chosen).
 * - Le macro DT_... diventano COSTANTI in compilazione: nessun costo a runtime, e un
 *   nome sbagliato e' un errore di compilazione (non un crash).
 * - Memory-mapped I/O: ogni periferica ha i suoi registri a un indirizzo fisso
 *   (la proprieta' reg). uart0 a 0x40002000, i2c0 a 0x40003000...
 * - 1048576 = 1024*1024 = 1 MB di flash; 262144 = 256 KB di RAM.
 *
 * COME VERIFICARE DA SOLO
 *     .\verifica.ps1 modulo2\04_devicetree_lettura
 *
 * ERRORI TIPICI
 * - Scrivere i nomi come nel .dts: DT_PROP(nodo, current-speed) o
 *   DT_CHOSEN(zephyr,console). Nelle macro: minuscolo e "_" (current_speed,
 *   zephyr_console).
 * - Errori lunghissimi con "DT_N_S_soc_S_..._P_..." : significa nodo o proprieta'
 *   inesistente. Cerca il nome nel messaggio e confrontalo con zephyr.dts.
 * - Stampare un indirizzo con %d: viene un numero decimale enorme e illeggibile.
 *
 * BONUS (facoltativo)
 * - Stampa il nome completo del nodo del LED con DT_NODE_FULL_NAME(DT_ALIAS(led0))
 *   e la sua proprieta' label con DT_PROP(DT_ALIAS(led0), label).
 * - Quanti LED ha la scheda? DT_CHILD_NUM(DT_PATH(leds))
 * - Il pin 13 della porta gpio0 corrisponde al bit 13 del registro OUT della GPIO
 *   (indirizzo 0x50000504). Lo userai nel Modulo 1, esercizio sui registri.
 *
 * Scrivi il tuo codice qui sotto.
 */

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/devicetree.h>

int main(void)
{
	/* TODO: passo 2 */

	return 0;
}
