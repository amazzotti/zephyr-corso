/*
 * Esercizio 1 (Modulo 2) - prj.conf e Kconfig
 *
 * Teoria: teoria/03_build_kconfig_devicetree.md, sezioni 1 e 2.
 *
 * OBIETTIVO
 * Capire che il firmware e' "configurato" in compilazione: cambiare un'opzione in
 * prj.conf cambia il firmware prodotto. Spegnere il banner di avvio, leggere dal codice
 * il valore di alcune opzioni e trovarle in build/zephyr/.config.
 *
 * PASSI
 * 1. Compila ed esegui lo scheletro:  .\esegui.ps1 modulo2\01_prj_conf
 *    Vedi il banner "*** Booting Zephyr OS build ... ***". Annota la tabella
 *    "Memory region" (FLASH usata) stampata alla fine della compilazione.
 * 2. Apri build/zephyr/.config con VS Code e cerca BOOT_BANNER. Poi in prj.conf
 *    aggiungi la riga che lo spegne:
 *        CONFIG_BOOT_BANNER=n
 *    Ricompila ed esegui: il banner non c'e' piu'. Quanti byte di FLASH hai risparmiato?
 * 3. In main() stampa se il banner e' attivo, usando IS_ENABLED (vale 1 o 0):
 *        printk("Banner: %s\n", IS_ENABLED(CONFIG_BOOT_BANNER) ? "si" : "no");
 *    L'operatore  cond ? a : b  vale a se cond e' vera, altrimenti b (come
 *    "a if cond else b" in Python).
 * 4. Stampa due opzioni intere, che esistono sempre:
 *        "Tick al secondo: %d"   con CONFIG_SYS_CLOCK_TICKS_PER_SEC
 *        "Stack di main: %d byte" con CONFIG_MAIN_STACK_SIZE
 *    e una stringa: "Scheda: %s" con CONFIG_BOARD.
 * 5. Cerca in .config i valori che hai stampato: coincidono?
 * 6. Lancia la verifica.
 *
 * OUTPUT ATTESO (senza banner!)
 *     Banner: no
 *     Tick al secondo: 10000        (il numero puo' essere diverso)
 *     Stack di main: 1024 byte      (il numero puo' essere diverso)
 *     Scheda: stm32f4_disco
 *
 * CONCETTI NUOVI
 * - prj.conf: le TUE opzioni; tutte le altre prendono il valore predefinito della
 *   board e di Zephyr. Il risultato completo e' build/zephyr/.config.
 * - Ogni opzione attiva diventa una macro CONFIG_... visibile in tutto il codice.
 *   Un bool impostato a n NON definisce la macro: per questo si usa IS_ENABLED().
 * - Il "tick" e' l'unita' di tempo del kernel: sull'STM32F4 il tempo lo conta il
 *   SysTick, un timer interno al core Cortex-M che conta i cicli di clock (168 MHz,
 *   CONFIG_SYS_CLOCK_HW_CYCLES_PER_SEC) e genera un interrupt a ogni tick.
 * - Configurare = decidere in compilazione cosa entra nel firmware. Cio' che e'
 *   spento non occupa memoria: importante su un chip con 1 MB di flash.
 *
 * COME VERIFICARE DA SOLO
 *     .\verifica.ps1 modulo2\01_prj_conf
 * controlla anche che il banner NON compaia.
 *
 * ERRORI TIPICI
 * - Spazi in prj.conf ("CONFIG_BOOT_BANNER = n"): errore di parsing.
 * - Scrivere "#if CONFIG_BOOT_BANNER" quando l'opzione e' n: la macro non esiste.
 *   Usa IS_ENABLED().
 * - Modifiche a prj.conf che "non si vedono": esegui con -Pulito.
 * - Stampare una stringa con %d o un numero con %s: output sbagliato o crash.
 *
 * BONUS (facoltativo)
 * - Aggiungi CONFIG_PRINTK=n: cosa succede alla compilazione o all'output? Perche'?
 *   (Poi rimetti com'era.)
 * - In .config cerca CONFIG_SERIAL e CONFIG_UART_CONSOLE: chi li ha attivati, se in
 *   prj.conf non ci sono? (Suggerimento: i default della board, file
 *   zephyr/boards/st/stm32f4_disco/stm32f4_disco_defconfig.) E CONFIG_GPIO? Su
 *   questa scheda e' spento: per i LED andra' attivato a mano (esercizio 5).
 *
 * Scrivi il tuo codice qui sotto.
 */

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys/util.h> /* IS_ENABLED */

int main(void)
{
	/* TODO: passi 3 e 4 */

	return 0;
}
