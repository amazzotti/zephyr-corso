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
 *      - il nodo usart2 (serial@40004400): proprieta' reg e current-speed;
 *      - il nodo leds e il figlio led_4 (etichetta green_led_4): proprieta' gpios
 *        (porta, pin, flag);
 *      - i nodi puntati da zephyr,flash e zephyr,sram: proprieta' reg;
 *      - il nodo i2c1.
 *    Rispondi alle domande in risposte.md.
 * 2. Nel codice stampa, con le macro della teoria (sezione 6):
 *      "Velocita console: %d"     DT_PROP(DT_CHOSEN(zephyr_console), current_speed)
 *      "Indirizzo usart2: 0x%08x" DT_REG_ADDR(DT_NODELABEL(usart2))
 *      "LED0 sul pin: %d"         DT_GPIO_PIN(DT_ALIAS(led0), gpios)
 *      "Flash: %d byte"           DT_REG_SIZE(DT_CHOSEN(zephyr_flash))
 *      "Flash da: 0x%08x"         DT_REG_ADDR(DT_CHOSEN(zephyr_flash))
 *      "RAM: %d byte"             DT_REG_SIZE(DT_CHOSEN(zephyr_sram))
 *      "Indirizzo i2c1: 0x%08x"   DT_REG_ADDR(DT_NODELABEL(i2c1))
 *    Gli indirizzi conviene convertirli: (uint32_t)DT_REG_ADDR(...) e stamparli con
 *    %08x (8 cifre esadecimali con zeri davanti).
 * 3. In Renode scrivi  peripherals  nel monitor: trovi gli stessi indirizzi di usart2
 *    e di i2c1? Il devicetree descrive esattamente l'hardware che Renode simula.
 * 4. Lancia la verifica.
 *
 * OUTPUT ATTESO
 *     Velocita console: 115200
 *     Indirizzo usart2: 0x40004400
 *     LED0 sul pin: 12
 *     Flash: 1048576 byte
 *     Flash da: 0x08000000
 *     RAM: 131072 byte
 *     Indirizzo i2c1: 0x40005400
 *
 * CONCETTI NUOVI
 * - Devicetree: descrizione dell'hardware (nodi, proprieta', etichette, alias, chosen).
 * - Le macro DT_... diventano COSTANTI in compilazione: nessun costo a runtime, e un
 *   nome sbagliato e' un errore di compilazione (non un crash).
 * - Memory-mapped I/O: ogni periferica ha i suoi registri a un indirizzo fisso
 *   (la proprieta' reg). usart2 a 0x40004400, i2c1 a 0x40005400, la porta GPIO D
 *   (dove sono i LED) a 0x40020c00...
 * - 1048576 = 1024*1024 = 1 MB di flash, che sull'STM32 parte da 0x08000000;
 *   131072 = 128 KB di RAM principale (da 0x20000000). Lo STM32F407 ha anche 64 KB di
 *   RAM "CCM" (nodo ccm0, da 0x10000000), qui non usata come RAM principale.
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
 * - Il pin 12 della porta GPIO D corrisponde al bit 12 del registro ODR della porta
 *   (indirizzo 0x40020c14 = base 0x40020c00 + 0x14). Lo usi nel Modulo 1, esercizio
 *   sui registri.
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
